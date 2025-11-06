/********************************************************************************
 *                                                                              *
 * FitsIP - synthesize background from image                                    *
 *                                                                              *
 * modified: 2025-08-27                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) Harald Braeuning                                               *
 ********************************************************************************
 * This file is part of FitsIP.                                                 *
 * FitsIP is free software: you can redistribute it and/or modify it            *
 * under the terms of the GNU General Public License as published by the Free   *
 * Software Foundation, either version 3 of the License, or (at your option)    *
 * any later version.                                                           *
 * FitsIP is distributed in the hope that it will be useful, but                *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                                *
 * You should have received a copy of the GNU General Public License along with *
 * FitsIP. If not, see <https://www.gnu.org/licenses/>.                         *
 ********************************************************************************/

#ifndef SYNTHESIZEBACKGROUND_H
#define SYNTHESIZEBACKGROUND_H

#include <fitsip/core/fitsimage.h>
#include <fitsip/core/opplugin.h>
#include <QObject>
#include <vector>
#include <random>

class SynthesizeBackgroundDialog;

/**
 * @brief Plugin to synthesize the sky background.
 *
 * Synthesize the sky background as a polynomial of arbitrary order, though
 * order 2 is usually good. The algorithm is based on the algorithm and Pascal
 * code by Philippe Martinole. The synthesized sky background is stored in
 * a new ImageBuffer.
 *
 * @author hbr
 *
 */
class SynthesizeBackground: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)
public:
  SynthesizeBackground();
  ~SynthesizeBackground();

  virtual QString getMenuEntry() const override;

  virtual std::vector<std::shared_ptr<FitsObject>> getCreatedImages() const override;

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

//  void unsharpmask(std::shared_ptr<FitsImage> image, double sigma, double strength);

private:

  std::vector<Pixel> getRandomPoints(const FitsImage& image, uint32_t n, double bkg);
  std::vector<Pixel> getGridPoints(const FitsImage& image, uint32_t n, double bkg);
  std::vector<double> getCoefficients(const std::vector<Pixel>& pixels, uint32_t deg, uint32_t layer);
  FitsImage createImage(u_int32_t w, uint32_t h, std::vector<std::vector<double>> coeff, uint32_t deg);

  FitsImage img;
  SynthesizeBackgroundDialog* dlg;

  static std::mt19937 rng;

};

#endif // SYNTHESIZEBACKGROUND_H
