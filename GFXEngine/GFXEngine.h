/**
 *  @file   GFXEngine.h
 *  @brief  3D Graphics Engine in the NCursesGameEngine
 *  @author KrizTioaN (christiaanboersma@hotmail.com)
 *  @date   2021-07-29
 *  @note   BSD-3 licensed
 *
 ***********************************************/

#include "../GFXToolkit.h"
#include "../NCursesGameEngine.h"

#include <codecvt>
#include <iostream>
#include <locale>

#ifndef CBNCURSESGAMEENGINE_GFXENGINE_H
#define CBNCURSESGAMEENGINE_GFXENGINE_H

class GFXEngine : public cb::NCursesGameEngine {

public:
  GFXEngine(const int argc, const char *argv[]) {
    if (argc > 1) {
      std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
      model = conv.from_bytes(argv[1]);
    } else {
      std::cerr << "please provide a model\n";
      exit(1);
    }
  }

  bool OnUserCreate() final {

    vCamera = {0.0f, 0.0f, 0.0f};

    fAngle = 0.0f;

    mProj = ProjectionMatrix(0.1f, 1000.0f, 90.0f,
                             (float)ScreenHeight() / (float)ScreenWidth()) *
            TranslationMatrix(1.0f, 1.0f, 1.0f) *
            ScalingMatrix(0.5f * (float)ScreenWidth(),
                          0.5f * (float)ScreenHeight(), 1.0f);

    mTrans = TranslationMatrix(0.0f, 0.0f, 8.0f);

    vIllumination = {0.0f, 0.0f, -1.0f};

    // vIllumination.normalize();

    return mObj.ReadObj(model);
  }

  bool OnUserUpdate(float fElapsedTime) final {

    if (IsFocused() && KeyUp(kVK_ANSI_Q))
      return false;

    fAngle += fElapsedTime;

    Clear(PIXEL_FULL, FG_BLACK);

    cb::mat4x4 mWorld =
        RotationMatrixZ(fAngle) * RotationMatrixX(fAngle * 0.5f) * mTrans;

    std::vector<cb::triangle3d> vRasterize;

    for (auto t : mObj.triangles) {

      t = mWorld * t;
 
      cb::vec3d normal = (t.p2 - t.p1).cross(t.p3 - t.p1).normalize();

      float fDot = normal * (t.p1 - vCamera);

      if (fDot >= 0.0f)
        continue;

      fDot = normal * vIllumination;

      t = mProj * t;

      t.color = FG_GREY1 + (int)(24.0f * std::fabs(fDot));

      vRasterize.emplace_back(t);
    }

    std::sort(vRasterize.begin(), vRasterize.end(),
              [](const cb::triangle3d &a, const cb::triangle3d &b) {
                return ((a.p1.z + a.p2.z + a.p3.z)) <
                       ((b.p1.z + b.p2.z + b.p3.z));
              });

    for (auto t : vRasterize) {

      DrawFilledTriangle((int)t.p1.x, (int)t.p1.y, (int)t.p2.x, (int)t.p2.y,
                         (int)t.p3.x, (int)t.p3.y, PIXEL_FULL, t.color);
    }

    return true;
  }

  std::wstring model;

  cb::mesh mObj;

  float fAngle;

  cb::mat4x4 mProj;

  cb::mat4x4 mTrans;

  cb::vec3d vCamera;

  cb::vec3d vIllumination;
};

#endif // CBNCURSESGAMEENGINE_GFXENGINE_H
