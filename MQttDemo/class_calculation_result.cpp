#include "class_calculation_result.h"

class_calculation_result::class_calculation_result(QObject *parent) :
    QObject(parent)
{
}


bool class_calculation_result::class_copy(class_calculation_result &calss_result){
    m_file_name = calss_result.m_file_name;
    return true;
}
