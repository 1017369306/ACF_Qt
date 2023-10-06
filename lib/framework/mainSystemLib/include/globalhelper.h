#ifndef GLOBALHELPER_H
#define GLOBALHELPER_H

#include "mainSystemLib_global.h"
#include <QObject>
#include <QWidget>
#include <QFileInfoList>
#include <QMainWindow>
#include <QPixmap>
#include <QVariant>
#include <QRectF>
#include <QColor>
#include <QDomElement>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QTextCodec>
#include <QDateTime>

/**
 * @brief The GlobalMethods class
 * 作者 zhangyang
 * 日期 2022-4-19
 */
class MAINSYSTEMLIB_EXPORT GlobalMethods : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief removeLayoutAllItem 清空widget的layout布局内的所有元素
     * @param widget
     */
    static void removeLayoutAllItem(QWidget *widget);

    static void foreachUpward(QWidget *widget);

    static void foreachDownward(QWidget *widget);

    static QFileInfoList foreachDir(const QString &path, const QStringList &nameFilters);

    /**
     * @brief findFile 查找当前路径下的文件
     * @param path
     * @param nameFilters
     * @return
     */
    static QFileInfoList findFile(const QString &path, const QStringList &nameFilters)
    {
        // 获取所有文件夹名
        QDir dir(path);
//        path = dir.fromNativeSeparators(path);//  "\\"转为"/"
        QStringList allFile = QStringList("");
        dir.setFilter(QDir::Files | QDir::Readable);
        dir.setSorting(QDir::Name);
        QFileInfoList files = dir.entryInfoList(nameFilters);
        return  files;
    }

    /**
     * @brief findFolder 查找当前路径下的子目录
     * @param folder
     * @return
     */
    static QStringList findFolder(const QString &folder)
    {
        // 获取所有文件夹名
        QDir dir(folder);
//        folder = dir.fromNativeSeparators(folder);//  "\\"转为"/"
        QStringList allFolder = QStringList("");
        dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::Name);
        allFolder = dir.entryList();
        return  allFolder;
    }

    /**
     * @brief findAllFiles 递归查找当前路径下所有文件
     * @param folder
     * @param nameFilters
     * @return
     */
    static QFileInfoList findAllFiles(const QString &folder, const QStringList &nameFilters)
    {
        QFileInfoList list;
        //添加当前路径下所有的文件
        QFileInfoList files = findFile(folder, nameFilters);
        for(int i=0; i<files.size(); i++)
        {
            list.append(files.at(i));
        }
        // 获取所有文件夹名
        QStringList folders = findFolder(folder);        //根目录下的子文件夹
        //遍历子文件夹 获取里面的文件和文件夹
        for (int i = 0; i < folders.size(); ++i)
        {
            list.append(findAllFiles(folder + "/" + folders.at(i), nameFilters));
        }
        return list;
    }

    static bool getIPAndPort(const QString &uri, QString &ip, int &port);

    static QString stdToQString(const std::wstring &str);

    static QMainWindow *getMainWindow();

    static QString getMarginsStr(const QRectF &margins){
        QString top = QString(" %1px").arg(margins.y());
        QString right = QString(" %1px").arg(margins.width());
        QString bottom = QString(" %1px").arg(margins.height());
        QString left = QString(" %1px").arg(margins.x());

        QString str = top + right + bottom + left;//qss中规则为top-right-bottom-left
        return str;
    }

    static QString getShowValue(const QVariant &value){
        QString str;
        switch (value.type()) {
        case QVariant::Type::RectF:
        {
            str = getMarginsStr(value.toRectF());
            break;
        }
        default:
        {
            str = value.toString();
            break;
        }
        }
        return str;
    }

private:

    static void debugInfo(QWidget *widget);

};

/**
 * @brief The LogDataModel struct 日志结构体
 */
struct MAINSYSTEMLIB_EXPORT LogDataModel
{
    QDateTime recordTime;
    QString logLevel= "";
    QString msg = "";
};

class MAINSYSTEMLIB_EXPORT LogReaderHelper
{
public:

    static QList<LogDataModel> read(const QString &fileName){
        QList<LogDataModel> entitys;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return entitys;
        }

        QTextCodec* textCodec = QTextCodec::codecForName("utf-8");
        while(!file.atEnd())
        {
            QByteArray line = file.readLine();
            QString lineString = textCodec->toUnicode(line);

            LogDataModel logDataModel;
            QStringList stringList = lineString.split(',');
            // 时间
            QString dateTimeStr = stringList.at(0);
            logDataModel.recordTime = QDateTime::fromString(dateTimeStr, "yyyy-MM-dd HH:mm:ss.zzz");

            int startIndex = lineString.indexOf("{");
            int endIndex = lineString.indexOf("}");
            QString jsonStr = lineString.mid(startIndex, endIndex);

            QJsonParseError jsonError;
            QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonStr.toStdString().data(), &jsonError));	//schoolInfo为上面这串数据的QString类型的变量
            if(jsonError.error == QJsonParseError::NoError){
                QJsonObject rootObj = jsonDoc.object();

                //------根据实际情况修改
                QString logType = rootObj.value("logType").toString();
                QString thread = rootObj.value("thread").toString();
                QString param = rootObj.value("param").toString();
                QString codePosition = rootObj.value("codePosition").toString();

                logDataModel.logLevel = logType;
                logDataModel.msg = param;
            }else {
                qDebug() << "json error!" << jsonError.errorString();	//json数据有问题
            }
            entitys.append(logDataModel);
        }

        file.close();
        return entitys;
    }
};

/**
 * @brief The FileHelper class 文件帮助类
 */
class MAINSYSTEMLIB_EXPORT FileHelper{
public:

    /**
     * @brief readFile 读取单个文件的内容
     * @param path
     * @return
     */
    static QByteArray readFile(const QString &path)
    {
        auto info = QFileInfo(path);
        if(!info.exists())
        {
            return QByteArray();
        }
        if(!info.isFile())
        {
            return QByteArray();
        }
        QFile styleFile(path);
        if(!styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return QByteArray();
        }
        auto buffer = styleFile.readAll();
        styleFile.close();
        return buffer;
    }

    /**
     * @brief readlFiles 读取多个文件的内容
     * @param list
     * @return
     */
    static QString readlFiles(const QStringList &list)
    {
        QStringList css;
        foreach(QString item, list)
        {
            auto style = readFile(item);
            if(!style.isEmpty())
            {
                css.append(style);
            }
        }
        return css.join("\n");
    }

};

/**
 * @brief The PixmapHelper class 图像处理帮助类
 */
class MAINSYSTEMLIB_EXPORT PixmapHelper{
public:

    static QPixmap loadSvg(const QString &path, const int &width = 22, const int &height = 22);

};

/**
 * @brief The IconHelper class Icon处理帮助类
 */
class MAINSYSTEMLIB_EXPORT IconHelper{
public:

    static QPixmap loadSvg(const QString &path, QColor &color, const int &width = 22, const int &height = 22){
        return IconHelper::maskToColorAndAlpha(PixmapHelper::loadSvg(path, width, height), color);
    }

    /**
     * @brief maskToColorAndAlpha 更换png图标的颜色（绘制出来的图标会失真）
     * @param mask 图标
     * @param color 将要更换的颜色
     * @return
     */
    static QPixmap maskToColorAndAlpha(const QPixmap &mask, QColor &color)
    {
//        QImage result(mask.toImage().convertToFormat(QImage::Format_ARGB32));
//        result.setDevicePixelRatio(mask.devicePixelRatio());
//        QRgb *bitsstart = reinterpret_cast<QRgb*>(result.bits());
//        const QRgb *bitsEnd = bitsstart + result.width() * result.height();
//        const QRgb tint = color.rgb() & 0x00ffffff;
//        const QRgb alpha = QRgb(color.alpha());
//        for(QRgb *pixel = bitsstart; pixel < bitsEnd;++pixel){
//            QRgb pixelAlpha = (((~*pixel) & 0xff) * alpha) >> 8;
//            *pixel = (pixelAlpha << 24) | tint;
//        }
//        return QPixmap::fromImage(result);

        QImage image = mask.toImage().convertToFormat(QImage::Format_RGBA8888_Premultiplied, Qt::NoFormatConversion);
        int bmpWidth = image.width();
        int bmpHeight = image.height();
        for(int i=0;i< bmpWidth;++i)
        {
            for(int j=0;j<bmpHeight;++j)
            {
                //将灰色(0~255)全部替换成设定的颜色，全透明的不替换
                QColor tempColor = image.pixelColor(i,j);
                if(tempColor.alpha() != 0)
                {
                    color.setAlpha(tempColor.alpha());
                    image.setPixelColor(i,j,color);
                }
            }
        }
        return QPixmap::fromImage(image);
    }

    /**
     * @brief recolor 把背景色和前景色结合在一起
     * QColor foreground = QColor("#FF0000");
     * QColor background = QColor("#0000FF");
     * recolor(&image, foreground, background);
     * @param image
     * @param foreground
     * @param background
     */
    static void recolor(QImage *image, const QColor &foreground, const QColor &background)
    {
        if (image->format() != QImage::Format_ARGB32_Premultiplied) {
            // qCWarning(OkularUiDebug) << "Wrong image format! Converting...";
            *image = image->convertToFormat(QImage::Format_ARGB32_Premultiplied);
        }

        Q_ASSERT(image->format() == QImage::Format_ARGB32_Premultiplied);

        const float scaleRed = background.redF() - foreground.redF();
        const float scaleGreen = background.greenF() - foreground.greenF();
        const float scaleBlue = background.blueF() - foreground.blueF();

        for (int y=0; y<image->height(); y++) {
            QRgb *pixels = reinterpret_cast<QRgb*>(image->scanLine(y));

            for (int x=0; x<image->width(); x++) {
                const int lightness = qGray(pixels[x]);
                pixels[x] = qRgba(scaleRed * lightness + foreground.red(),
                               scaleGreen * lightness + foreground.green(),
                               scaleBlue * lightness + foreground.blue(),
                               qAlpha(pixels[x]));
            }
        }
    }

};

/**
 * @brief The ColorHelper class color帮助类
 */
class MAINSYSTEMLIB_EXPORT ColorHelper{

public:

    /**
     * @brief getHexRgb QColor转16进制rgb字符串
     * @param color
     * @return
     */
    static QString getHexRgb(const QColor &color){
//        QColor color("#FF0000");
        QString colorStr = color.name(QColor::HexRgb);
        return colorStr;
    }
};

/**
 * @brief The SvgHelper class SVG矢量图帮助类
 */
class MAINSYSTEMLIB_EXPORT SvgHelper{

public:

    /**
     * @brief DealJson 解析json内容的函数
     * @param _qstrJson json文件路径
     * @param _qstrSkin 第一层主题节点
     * @param _qstrImagePath 第二层svg路径节点
     */
    static void DealJson(QString _qstrJson,QString _qstrSkin, QString _qstrImagePath)
    {
        //读取json配置文件
        QFile fileJson(_qstrJson);
        fileJson.open(QIODevice::ReadOnly);
        QJsonDocument jsondoc = QJsonDocument::fromJson(fileJson.readAll());  //注意这里是怎么赋值的
        QJsonObject obj = jsondoc.object(); //第一层

        if(obj.contains(_qstrSkin))
        {
            QJsonObject objFileName = obj[_qstrSkin].toObject(); //第二层
            QStringList listFileName = objFileName.keys();        //这里获取的都是文件名称
            for(int i = 0; i < listFileName.size(); ++i)
            {
               QString qstrFileName = listFileName[i];
               QString qstrFilePath  = _qstrImagePath + qstrFileName+".svg";  //配置要修改的svg文件路径
               QFile fileSvg(qstrFilePath);
               fileSvg.open(QIODevice::ReadOnly);
               QByteArray baData = fileSvg.readAll();
               fileSvg.close();
               QDomDocument doc;
               doc.setContent(baData);
               QDomElement element = doc.documentElement();

               QJsonObject objLabel = objFileName[qstrFileName].toObject(); //第三层

               QStringList listLabel = objLabel.keys();                     //这里获取的是标签名
               for(int j = 0; j < listLabel.size(); ++j)
               {
                   QString qstrLabel = listLabel[j];
                   if(objLabel[qstrLabel].isArray()) //如果是数组
                   {
                       QJsonArray arrayAttribute = objLabel[qstrLabel].toArray(); //第四层
                       for(int q = 0; q < arrayAttribute.size(); ++q)
                       {
                           QJsonObject objAttribute = arrayAttribute[q].toObject();        //第五层
                           QStringList listAttribute = objAttribute.keys();

                           QString qstrId = "";
                           if(objAttribute.contains("id"))
                           {
                               qstrId = objAttribute["id"].toString();
                           }
                           for(int p = 0; p < listAttribute.size(); ++p)
                           {
                                QString key = listAttribute[p];
                                QString value = objAttribute[key].toString();
                                SetAttrRecur(element, qstrLabel, qstrId, key, value);
                                fileSvg.setFileName(qstrFilePath);
                                fileSvg.open(QIODevice::WriteOnly);
                                fileSvg.write(doc.toByteArray());
                                fileSvg.close();
                           }
                       }
                   }
                   else if(objLabel[qstrLabel].isObject()) //如果是对象
                   {
                       QJsonObject objAttribute = objLabel[qstrLabel].toObject();
                       QStringList listAttribute = objAttribute.keys(); //这里获取的是属性名称
                       for(int k = 0; k < listAttribute.size(); ++k)
                       {
                           QString key = listAttribute[k];
                           QString value = objAttribute[key].toString();
                           SetAttrRecur(element, qstrLabel,"", key, value);
                           fileSvg.setFileName(qstrFilePath);
                           fileSvg.open(QIODevice::WriteOnly);
                           fileSvg.write(doc.toByteArray());
                           fileSvg.close();
                       }
                   }
               }
            }
        }
    }

    static void updateAllSvg(const QString svgDirPath, const QString &iconColor, const QString &primaryColor)
    {
        QStringList filters{"*.svg"};
        //递归获取所有的svg图片
        QFileInfoList svgInfos =  GlobalMethods::findAllFiles(svgDirPath, filters);
        for(QFileInfo svgInfo : svgInfos){
            QString qstrFilePath  = svgInfo.filePath();

            //svg文件名带下划线的，颜色用主题色，否则用图标颜色
            bool isPrimaryColor = svgInfo.fileName().contains("_");
            updateSvg(qstrFilePath, isPrimaryColor ? primaryColor : iconColor);

            qDebug() << svgInfo.fileName() << " color:" << (isPrimaryColor ? primaryColor : iconColor);
        }
    }

    static void updateSvg(const QString svgDirPath, const QString &color){
        QFile fileSvg(svgDirPath);
        fileSvg.open(QIODevice::ReadOnly);
        //读取svg文件中所有字符
        QByteArray baData = fileSvg.readAll();
        fileSvg.close();
        QDomDocument doc;
        doc.setContent(baData);
        QDomElement element = doc.documentElement();

        //svg文件名带下划线的，颜色用主题色，否则用图标颜色
        SetAttrRecur(element, "path", "", "fill", color);

        //修改fill值后，写入svg文件中
        fileSvg.setFileName(svgDirPath);
        fileSvg.open(QIODevice::WriteOnly);
        fileSvg.write(doc.toByteArray());
        fileSvg.close();
    }

private:

    //这是借鉴其他博主的作用是修改svg内容的
    static void SetAttrRecur(QDomElement &elem, QString strtagname, QString id ,QString strattr, QString strattrval)
    {
        if (elem.tagName().compare(strtagname) == 0) //当找到了要修改的对象 就设置属性
        {
           if(id != "" && elem.attribute("id") == id)
           {
               elem.setAttribute(strattr, strattrval);
           }
           else if(id == "")
           {
               elem.setAttribute(strattr, strattrval);
           }
        }

        //循环查找
        for (int i = 0; i < elem.childNodes().count(); i++)
        {
            if (!elem.childNodes().at(i).isElement()) //如果不是元素
            {
                continue;
            }

            QDomElement element = elem.childNodes().at(i).toElement();
            SetAttrRecur(element, strtagname, id, strattr, strattrval);
        }
    }

};

#endif // GLOBALHELPER_H
