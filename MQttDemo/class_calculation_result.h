#ifndef CLASS_CALCULATION_RESULT_H
#define CLASS_CALCULATION_RESULT_H

#include <QObject>

#include <QDateTime>


class class_calculation_result : public QObject
{

public:
    explicit class_calculation_result(QObject *parent = 0);


    //原始数据统计后的结果
    double m_max[2];    //极大值
    double m_min[2];    //极小值
    double m_avr[2];    //均值
    double m_datel_T[2];   //时间差
    double m_drop[2][2];   //最大落差
    double m_variance[2];//标准方差

    //数据进行平滑之后的统计结果
    double m_max_after_smooth[2];
    double m_min_after_smooth[2];
    double m_avr_after_smooth[2];
    double m_datel_T_after_smooth[2];   //时间差
    double m_drop_after_smooth[2][2];   //最大落差
    double m_variance_after_smooth[2];//标准方差

    //数据进行归一化之后的统计结果
    double m_max_after_normalization[2];
    double m_min_after_normalization[2];
    double m_avr_after_normalization[2];
    double m_datel_T_after_normalization[2];   //时间差
    double m_drop_after_normalization[2][2];   //最大落差 第一个[2]是两条波形的意思，第二个[2]是[0]为X[1]为Y
    double m_variance_after_normalization[2];//标准方差

    //数据进行归一化之后的统计结果
    double m_max_after_smooth_and_normalization[2];
    double m_min_after_smooth_and_normalization[2];
    double m_avr_after_smooth_and_normalization[2];
    double m_datel_T_after_smooth_and_normalization[2];   //时间差
    double m_drop_after_smooth_and_normalization[2][2];   //最大落差
    double m_variance_after_smooth_and_normalization[2];//标准方差

    //2020年10月22日
    double m_datel_T_after_smooth_and_normalization_and_overturn[2];   //时间差--PD1波形进行翻转之后的时间差
    int m_count_zero_after_normalization[2];   //过零点
    int m_count_zero_after_smooth_and_normalization[2];   //过零点

    int m_count_A_after_normalization[2];   //过A线数量-归一化
    int m_count_A_after_smooth_and_normalization[2];   //过A线数量--平滑并归一化
    int m_count_B_after_normalization[2];   //过A线数量-归一化
    int m_count_B_after_smooth_and_normalization[2];   //过A线数量--平滑并归一化

    //2020年10月27日
    double m_confidence_last;   //最终的置信度
    double m_delta_T_all;   //时间差
    double m_similary_all_min;   //最小的相似度
    int m_warning_level;            //报警等级

    //以下是统计出来之后显示在界面的参数
    //编号
    //文件名
    //转换为编号
    //提取开始时间
    //位置
    //坐标
    //发生记录的时间
    //发生次数/小时
    //事件判断(名称)
    //建议处理方式（待增加功能）
    //处理方式

    //事件顺序号
    QString m_file_name;
    double m_id;
    QDateTime m_dateTime_start;
    QDateTime m_dateTime_end;
    int m_pos_map[2];
    double m_pos_GPS[2];
    double m_lenght_all;    //总的长度
    QString m_string_warning_name;  //报警名称

    int m_sort_id;

    int m_OSW_model_when_warning;

    QList<QDateTime > m_list_dateTime_record;

    bool class_copy(class_calculation_result &calss_result);

};

#endif // CLASS_CALCULATION_RESULT_H
