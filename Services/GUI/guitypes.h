#pragma once
#include <QString>
#include <QStringList>

enum class WidgetTypes {
    INFO,
    VARIANT,
    WARNING
};

struct WidgetInfo {
    QString title;
    QStringList information;

    //QMap<int, VARIANT_DATA_VAR_DIRECTIVE> variants; //only_for_variant_direct

    void clear() {
        title.clear();
        information.clear();
        //variants.clear();
    }
};

struct WindowResultData {
    bool status;
    QString data;
};

struct TitleOfSectionDescription {
    QString title_of_section;
    QString name_of_prot;
    QString traf;
    QString rep;
    QString operator_pu_ais;
    QString operator_ais;
    QString operator_tkck_ss_bvs;
    QString operator_ntk_si;
    QString operator_bs;

    void clear() {
        title_of_section.clear();
        name_of_prot.clear();
        traf.clear();
        rep.clear();
        operator_pu_ais.clear();
        operator_ais.clear();
        operator_tkck_ss_bvs.clear();
        operator_ntk_si.clear();
        operator_bs.clear();
    }
};
