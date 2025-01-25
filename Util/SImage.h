#ifndef SIMAGE_H
#define SIMAGE_H

#include <QObject>

class QLabel;
class SImage :public QObject
{
    Q_OBJECT
public:
    enum class LoadInWhere{
        HomePage_JointWidget,
        Other,
    };
    SImage();
    ~SImage();

    //异步：加载并裁剪图片到预览框
    static void loadAndCropImage(const QString& filePath, QLabel* pixmapLab, LoadInWhere loadTpye = LoadInWhere::Other);

    //同步：加载并裁剪图片，并返回QPixmap
    static const QPixmap loadAndCropImage(const QString& filePath, const QRect& previewRect);
private:
    static const QRect cropPixmap(const QPixmap& pixmap, const QRect& previewRect);
};

#endif // SIMAGE_H