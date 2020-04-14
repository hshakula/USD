//
// Copyright 2019 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "pxr/pxr.h"
#include "pxr/usdImaging/usdAppUtils/renderSettings.h"

#include "pxr/base/tf/diagnostic.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/scope.h"

PXR_NAMESPACE_OPEN_SCOPE

static UsdRenderSettings GetDefaultRenderSettings(const UsdStagePtr& stage) {
    if (auto metadataRenderSettings = UsdRenderSettings::GetStageRenderSettings(stage)) {
        return metadataRenderSettings;
    }

    auto renderScope = UsdGeomScope::Get(stage, SdfPath("/Render"));
    if (!renderScope) {
        return UsdRenderSettings();
    }

    auto primRange = UsdPrimRange(renderScope.GetPrim());
    for (const UsdPrim& usdPrim : primRange) {
        const UsdRenderSettings renderSettings(usdPrim);
        if (renderSettings) {
            return renderSettings;
        }
    }

    return UsdRenderSettings();
}

static UsdRenderSettings GetRenderSettingsAtPath(
    const UsdStagePtr& stage,
    const SdfPath& path) {
    if (!path.IsPrimPath()) {
        return UsdRenderSettings();
    }

    SdfPath usdPath = path;

    if (!path.IsAbsolutePath()) {
        if (path.GetPathElementCount() > 1u) {
            TF_RUNTIME_ERROR("RenderSettings path \"%s\" is not absolute", path.GetText());
            return UsdRenderSettings();
        } else {
            usdPath = SdfPath("/Render").AppendPath(path);
        }
    }

    return UsdRenderSettings::Get(stage, usdPath);
}

UsdRenderSettings UsdAppUtilsGetRenderSettings(
    const UsdStagePtr& stage,
    std::vector<SdfPath> const& paths) {
    if (!stage) {
        TF_CODING_ERROR("Invalid stage");
        return UsdRenderSettings();
    }

    if (paths.empty()) {
        return GetDefaultRenderSettings(stage);
    } else if (paths.size() == 1) {
        return GetRenderSettingsAtPath(stage, paths[0]);
    } else {
        // Create a new RenderSettings prim in the session layer,
        // and reference the input RenderSettings prims
        SdfPath combinationPath("/Render/tmp/RenderSettings");
        UsdRenderSettings renderSettings = UsdRenderSettings::Define(stage, combinationPath);
        UsdPrim renderSettingsPrim = renderSettings.GetPrim();
        UsdReferences references = renderSettingsPrim.GetReferences();
        references.ClearReferences();

        bool anyReferenceCreated = false;
        for (auto& path : paths) {
            if (auto referencedRenderSettings = GetRenderSettingsAtPath(stage, path)) {
                if (references.AddInternalReference(path)) {
                    anyReferenceCreated = true;
                }
            }
        }

        return anyReferenceCreated ? renderSettings : GetDefaultRenderSettings(stage);
    }
}

void UsdAppUtilsSetRendererSettings(
        UsdImagingGLEngine& engine,
        UsdRenderSettings const& renderSettings,
        UsdTimeCode const& timeCode) {
    // Set all renderer settings to default
    auto renderSettingsList = engine.GetRendererSettingsList();
    for (auto renderSetting : renderSettingsList) {
        engine.SetRendererSetting(renderSetting.key, renderSetting.defValue);
    }

    if (renderSettings) {
        UsdPrim renderSettingsPrim = renderSettings.GetPrim();
        auto authoredProperties = renderSettingsPrim.GetAuthoredProperties();
        for (auto& property : authoredProperties) {
            if (property.Is<UsdAttribute>()) {
                UsdAttribute attr = property.As<UsdAttribute>();

                VtValue value;
                if (attr.Get(&value, timeCode) && !value.IsEmpty()) {
                    engine.SetRendererSetting(property.GetName(), value);
                }
            }
        }
    }
}

PXR_NAMESPACE_CLOSE_SCOPE
