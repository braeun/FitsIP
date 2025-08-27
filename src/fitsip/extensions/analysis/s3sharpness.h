#ifndef S3SHARPNESS_H
#define S3SHARPNESS_H

#include <fitsip/core/opplugin.h>
#include <fitsip/core/dialogs/imageselectdialog.h>
#include <fitsip/core/math/linearregression.h>
#include <QObject>
#include <vector>
#include <fftw3.h>

class S3SharpnessResultDialog;

struct S3SharpnessData
{
  QFileInfo info;
  std::string filename;
  double s3 = 0;
  std::vector<std::shared_ptr<FitsObject>> images;
};

/**
 * @brief The S3Sharpness class
 *
 * Cuong T. Vu, Thien D. Phan, Damon M. Chandler.
 * S3: A Spectral and Spatial Measure of Local Perceived Sharpness in Natural Images.
 * IEEE Transactions on Image Processing 2011.
 */
class S3Sharpness: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)
public:
  S3Sharpness();
  virtual ~S3Sharpness();

  virtual std::vector<std::shared_ptr<FitsObject>> getCreatedImages() const override;

  virtual QString getMenuEntry() const override;

  virtual bool requiresFileList() const override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  virtual ResultType execute(const std::vector<QFileInfo>& list, const OpPluginData& data=OpPluginData()) override;

  virtual ResultType execute(const std::vector<std::shared_ptr<FitsObject>>& list, const OpPluginData& data=OpPluginData()) override;

private:
  S3SharpnessData evaluate(const QFileInfo info, QRect selection);
  S3SharpnessData calculateSharpness(FitsImage* img, double t1, double t2) const;
  std::pair<Layer*,std::vector<XYData>> calculateSpectralSharpness(Layer* layer, double t1, double t2) const;
  std::pair<LinearRegression,XYData> getSlope(fftw_complex* c, int m) const;
  Layer* calculateSpatialSharpness(Layer* layer) const;
  std::unique_ptr<Layer> calculateContrast(Layer* layer, int m, double t1, double t2) const;
  void copyToLog();

  std::vector<std::shared_ptr<FitsObject>> images;
  std::vector<S3SharpnessData> results;
  S3SharpnessResultDialog* resultDialog;
  int spectralBlocksize;
  int spectralOverlap;
  int spatialBlocksize;
  double sigmoid_t1;
  double sigmoid_t2;
  double contrast_t1;
  double contrast_t2;
  ValueType alpha;

};

#endif // S3SHARPNESS_H
