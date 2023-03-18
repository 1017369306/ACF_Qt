#ifndef GLOBALCOLORS_H
#define GLOBALCOLORS_H

#include <QObject>

/**
 * @brief The Globalstatisdata class 全局静态类库，用于存放运行时的全局共用的静态实例对象，如网络模块、设备控制协议模块等
 * 作者 zhangyang
 * 日期 2022-05-25
 */
class GlobalColors : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString LightPrimaryColor READ LightPrimaryColor WRITE setLightPrimaryColor)
    Q_PROPERTY(QString PrimaryColor READ PrimaryColor WRITE setPrimaryColor)
    Q_PROPERTY(QString DarkPrimaryColor READ DarkPrimaryColor WRITE setDarkPrimaryColor)
    Q_PROPERTY(QString LightDangerColor READ LightDangerColor WRITE setLightDangerColor)
    Q_PROPERTY(QString DangerColor READ DangerColor WRITE setDangerColor)
    Q_PROPERTY(QString DarkDangerColor READ DarkDangerColor WRITE setDarkDangerColor)
    Q_PROPERTY(QString LightWarningColor READ LightWarningColor WRITE setLightWarningColor)
    Q_PROPERTY(QString WarningColor READ WarningColor WRITE setWarningColor)
    Q_PROPERTY(QString DarkWarningColor READ DarkWarningColor WRITE setDarkWarningColor)
    Q_PROPERTY(QString LightInfoColor READ LightInfoColor WRITE setLightInfoColor)
    Q_PROPERTY(QString InfoColor READ InfoColor WRITE setInfoColor)
    Q_PROPERTY(QString DarkInfoColor READ DarkInfoColor WRITE setDarkInfoColor)
    Q_PROPERTY(QString LightSuccessColor READ LightSuccessColor WRITE setLightSuccessColor)
    Q_PROPERTY(QString SuccessColor READ SuccessColor WRITE setSuccessColor)
    Q_PROPERTY(QString DarkSuccessColor READ DarkSuccessColor WRITE setDarkSuccessColor)
    Q_PROPERTY(QString PrimaryTextColor READ PrimaryTextColor WRITE setPrimaryTextColor)
    Q_PROPERTY(QString SecondaryTextColor READ SecondaryTextColor WRITE setSecondaryTextColor)
    Q_PROPERTY(QString ThirdlyTextColor READ ThirdlyTextColor WRITE setThirdlyTextColor)
    Q_PROPERTY(QString ReverseTextColor READ ReverseTextColor WRITE setReverseTextColor)
    Q_PROPERTY(QString TextIconColor READ TextIconColor WRITE setTextIconColor)
    Q_PROPERTY(QString BorderColor READ BorderColor WRITE setBorderColor)
    Q_PROPERTY(QString SecondaryBorderColor READ SecondaryBorderColor WRITE setSecondaryBorderColor)
    Q_PROPERTY(QString BackgroundColor READ BackgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QString RegionColor READ RegionColor WRITE setRegionColor)
    Q_PROPERTY(QString SecondaryRegionColor READ SecondaryRegionColor WRITE setSecondaryRegionColor)
    Q_PROPERTY(QString ThirdlyRegionColor READ ThirdlyRegionColor WRITE setThirdlyRegionColor)
    Q_PROPERTY(QString TitleColor READ TitleColor WRITE setTitleColor)
    Q_PROPERTY(QString SecondaryTitleColor READ SecondaryTitleColor WRITE setSecondaryTitleColor)
    Q_PROPERTY(QString DarkMaskColor READ DarkMaskColor WRITE setDarkMaskColor)
    Q_PROPERTY(QString DarkOpacityColor READ DarkOpacityColor WRITE setDarkOpacityColor)

public:
//    explicit GlobalColors();

    static GlobalColors *instance(){
        if(m_instance == nullptr)
            m_instance = new GlobalColors();
        return m_instance;
    }

    QString LightPrimaryColor() const;
    void setLightPrimaryColor(const QString &LightPrimaryColor);

    QString PrimaryColor() const;
    void setPrimaryColor(const QString &PrimaryColor);

    QString DarkPrimaryColor() const;
    void setDarkPrimaryColor(const QString &DarkPrimaryColor);

    QString LightDangerColor() const;
    void setLightDangerColor(const QString &LightDangerColor);

    QString DangerColor() const;
    void setDangerColor(const QString &DangerColor);

    QString DarkDangerColor() const;
    void setDarkDangerColor(const QString &DarkDangerColor);

    QString LightWarningColor() const;
    void setLightWarningColor(const QString &LightWarningColor);

    QString WarningColor() const;
    void setWarningColor(const QString &WarningColor);

    QString DarkWarningColor() const;
    void setDarkWarningColor(const QString &DarkWarningColor);

    QString LightInfoColor() const;
    void setLightInfoColor(const QString &LightInfoColor);

    QString InfoColor() const;
    void setInfoColor(const QString &InfoColor);

    QString DarkInfoColor() const;
    void setDarkInfoColor(const QString &DarkInfoColor);

    QString LightSuccessColor() const;
    void setLightSuccessColor(const QString &LightSuccessColor);

    QString SuccessColor() const;
    void setSuccessColor(const QString &SuccessColor);

    QString DarkSuccessColor() const;
    void setDarkSuccessColor(const QString &DarkSuccessColor);

    QString PrimaryTextColor() const;
    void setPrimaryTextColor(const QString &PrimaryTextColor);

    QString SecondaryTextColor() const;
    void setSecondaryTextColor(const QString &SecondaryTextColor);

    QString ThirdlyTextColor() const;
    void setThirdlyTextColor(const QString &ThirdlyTextColor);

    QString ReverseTextColor() const;
    void setReverseTextColor(const QString &ReverseTextColor);

    QString TextIconColor() const;
    void setTextIconColor(const QString &TextIconColor);

    QString BorderColor() const;
    void setBorderColor(const QString &BorderColor);

    QString SecondaryBorderColor() const;
    void setSecondaryBorderColor(const QString &SecondaryBorderColor);

    QString BackgroundColor() const;
    void setBackgroundColor(const QString &BackgroundColor);

    QString RegionColor() const;
    void setRegionColor(const QString &RegionColor);

    QString SecondaryRegionColor() const;
    void setSecondaryRegionColor(const QString &SecondaryRegionColor);

    QString ThirdlyRegionColor() const;
    void setThirdlyRegionColor(const QString &ThirdlyRegionColor);

    QString TitleColor() const;
    void setTitleColor(const QString &TitleColor);

    QString SecondaryTitleColor() const;
    void setSecondaryTitleColor(const QString &SecondaryTitleColor);

    QString DarkMaskColor() const;
    void setDarkMaskColor(const QString &DarkMaskColor);

    QString DarkOpacityColor() const;
    void setDarkOpacityColor(const QString &DarkOpacityColor);

private:

    QString m_LightPrimaryColor = "#f3fbff";
    QString m_PrimaryColor = "#326cf3";
    QString m_DarkPrimaryColor = "#326cf3";

    QString m_LightDangerColor = "#fff6f7";
    QString m_DangerColor = "#db3340";
    QString m_DarkDangerColor = "#db3340";

    QString m_LightWarningColor = "#fffcf5";
    QString m_WarningColor = "#e9af20";
    QString m_DarkWarningColor = "#e9af20";

    QString m_LightInfoColor = "#f1fdff";
    QString m_InfoColor = "#00bcd4";
    QString m_DarkInfoColor = "#00bcd4";

    QString m_LightSuccessColor = "#f3fff6";
    QString m_SuccessColor = "#2db84d";
    QString m_DarkSuccessColor = "#2db84d";

    QString m_PrimaryTextColor = "#212121";
    QString m_SecondaryTextColor = "#757575";
    QString m_ThirdlyTextColor = "#bdbdbd";

    QString m_ReverseTextColor = "#212121";
    QString m_TextIconColor = "#ffffff";
    QString m_BorderColor = "#e0e0e0";
    QString m_SecondaryBorderColor = "#757575";
    QString m_BackgroundColor = "#eeeeee";
    QString m_RegionColor = "#ffffff";
    QString m_SecondaryRegionColor = "#eeeeee";
    QString m_ThirdlyRegionColor = "#ffffff";
    QString m_TitleColor = "#326cf3";
    QString m_SecondaryTitleColor = "#326cf3";
    QString m_DarkMaskColor = "#20000000";
    QString m_DarkOpacityColor = "#40000000";

private:

    static GlobalColors *m_instance;

};

#endif // GLOBALCOLORS_H
