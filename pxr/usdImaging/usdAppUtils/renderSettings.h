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
#ifndef PXR_USD_IMAGING_USD_APP_UTILS_RENDER_SETTINGS_H
#define PXR_USD_IMAGING_USD_APP_UTILS_RENDER_SETTINGS_H

/// \file usdAppUtils/renderSettings.h
///
/// Collection of module-scoped utilities for applications that operate using
/// USD cameras.

#include "pxr/pxr.h"
#include "pxr/usdImaging/usdAppUtils/api.h"

#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdRender/settings.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"


PXR_NAMESPACE_OPEN_SCOPE


/// Gets the UsdRenderSettings from the USD stage \p stage.
///
/// If \p paths is empty. If \p stage has default render setting,
/// as indicated by root layer metadata, it will return such,
/// otherwise one of the RenderSettings under "/Render" scope will be returned.
/// 
/// If \p paths contains one path. If this path is an absolute path, this is
/// equivalent to UsdRenderSettings::Get(), otherwise, if the path is a
/// single-element path representing just the name of a renderSettings prim,
/// then \p stage 's "/Render" will be searched for a UsdRenderSettings matching
/// that name. The UsdRenderSettings schema for that prim will be returned if found,
/// or an invalid UsdRenderSettings will be returned if not. Note that multi-element
/// relative path is invalid and invalid UsdRenderSettings will be returned.
///
/// If \p paths contains more than one path. All referenced RenderSettings primitives
/// would be combined into one RenderSettings primitive (via internal referencing).
/// If none of the provided paths are correct then default renderSettings returned.
USDAPPUTILS_API
UsdRenderSettings UsdAppUtilsGetRenderSettings(
        const UsdStagePtr& stage,
        std::vector<SdfPath> const& paths);

/// Sets render settings for the given timeCode with clearing all previous render settings
USDAPPUTILS_API
void UsdAppUtilsSetRendererSettings(
        UsdImagingGLEngine& engine,
        UsdRenderSettings const& renderSettings,
        UsdTimeCode const& timeCode = UsdTimeCode::Default());

PXR_NAMESPACE_CLOSE_SCOPE


#endif
