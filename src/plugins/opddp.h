/********************************************************************************
 *                                                                              *
 * FitsIP - digital development processing                                      *
 *                                                                              *
 * modified: 2022-11-20                                                         *
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

#ifndef OPDDP_H
#define OPDDP_H

#include <fitsbase/fitstypes.h>
#include <fitsbase/opplugin.h>
#include <QObject>
#include <vector>

#define QT_STATICPLUGIN
#include <QtPlugin>

class OpDDPDialog;


/** @page DDP Digital Development Processing
 *
 * The Digital Develpment Processing (DDP) by Kunihio Okano is based on the following
 * algorithm
 * @code
 * Y = [X+b]/[{X-bkg}+a]
 * @endcode
 * The braces {...} denote a blurring of the image by a gaussian with standard deviation
 * @a sigma. @a bkg is the sky background of the image and @a a and @a b are constants.
 *
 * For @a sigma a good staring point is 1. Smaller values (e.q. 0.5 - 0.75) work well
 * with stars and clusters, whereas larger values work well with fuzzy objects.
 * The @a a value must be larger than the background @a bkg subtracted from the image before
 * blurring. To sharpen star images the value should be relatively small, because the
 * central PVs in the stars are high and the gradient is steep. For example, if the
 * central PV in globular cluster stars is 700, then adding only 50 - 150 more then
 * @a bkg may be appropriate. However, if the image is M51 and the central PV in the
 * core is 700, it may be better to add an additional 300 - 500. The value @a b should be
 * rather small. A good starting point is 10 or 20. The sum of @a b and @a bkg must
 * not exceed the @a a value.
 */
class OpDDP: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)
public:
  OpDDP();
  ~OpDDP();

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsImage> image, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

  void ddp(std::shared_ptr<FitsImage> image, ValueType sigma, ValueType bkg, ValueType a, ValueType b);

private:
  OpDDPDialog* dlg;
};

#endif // OPDDP_H
