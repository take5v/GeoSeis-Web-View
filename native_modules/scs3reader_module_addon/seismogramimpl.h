#ifndef SEISMOGRAMIMPL_H
#define SEISMOGRAMIMPL_H

#include <string>

#define DATATYPE float

// typedefs from qt
typedef signed char qint8;         /* 8 bit signed */
typedef unsigned char quint8;      /* 8 bit unsigned */
typedef short qint16;              /* 16 bit signed */
typedef unsigned short quint16;    /* 16 bit unsigned */
typedef int qint32;                /* 32 bit signed */
typedef unsigned int quint32;      /* 32 bit unsigned */
typedef long long qint64;           /* 64 bit signed */
typedef unsigned long long quint64; /* 64 bit unsigned */

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;


// заголовак формата Sgy
struct StHeadingSgystHuct
{
    qint32 workNumber;                     // (1-4)     номер выполнения работы
    qint32 profileNumber;                  // (5-8)     номер профиля
    qint32 coilNumber;                     // (9-12)    номер катушки
    qint16 dataTraceNumber;             // (13-14)   количества трасс с данными в записи
    qint16 traceNumber;                 // (15-16)   количества вспомогательных трасс в записи
    qint16 stepDigMC_Coil;                 // (17-18)   шаг дискретизации в микросекундах ждя этой катушки
    qint16 stepDigMC_record;                // (19-20)   то же ждя исходной полевой записи
    qint16 numberDigTrace;                 // (21-22)   число дискретов в трассе для этой катушки
    qint16 numberDigTraceRecord;            // (23-24)   то же для исходной полевой записи
    qint16 codeFormatSeismicSets;          // (25-26)   Код формата сейсмических наборов:
    //           1 = IBM плавающая запятая (4 байта)
    //           2 = фиксированная запятая (4 байта)
    //           3 = фиксированная запятая (2 байта)
    //           4 = фиксированная запятая (без кода) (4 байта)
    //           5 = IEEE плавающая запятая (4 байта)  } НЕ СТАНДАРТНО!
    //           6 = фиксированная запятая (1 байт)       } Может быть переопределена в последующих версиях!
    qint16 frequencyRateOGT;                // (27-28)   Кратность ОГТ, ожидаемая для группы ОГТ
    qint16 codeSortingLines;                // (29-30)   Код сортировки трасс:
    //           1 = как записано (нет сортировки)
    //           2 = группа ОГТ
    //           3 = продолженный однократный профиль
    //           4 = горизонтально суммированы
    qint16 codeVerticalSum;             // (31-32)   Код вертикальной суммы:
    //           1 = нет суммы
    //           2 = две суммы
    //           N = n сумм
    qint16 frequencySvipaStart;         // (33-34)   Частота вибросейсмического свипа в начале
    qint16 frequencySvipaEnd;              // (35-36)    Частота вибросейсмического свипа в конце
    qint16 durationSvipa;                  // (37-38)    Длительность вибросейсмического свипа (мсек)
    qint16 typeCodeSvipa;                  // (39-40)    Код типа вибросейсмического свипа:
    //           1 = линейный
    //           2 = параболический
    //           3 = экспоненциальный
    //           4 = другой
    qint16 lineNumberChannelSvipa;         // (41-42)    Номер трассы в канале вибросейсмического свипа
    qint16 durationConeTraceSvipaStart; // (43-44)   Длительность конуса трассы вибросейсмического свипа в начале
    qint16 durationConeTraceSvipaEnd;      // (45-46)    Длительность конуса трассы вибросейсмического свипа в конце
    qint16 codeTypeConeTraceSvipa;         // (47-48)    Код типа конуса трассы вибросейсмического свипа:
    //           1 = линейный
    //           2 = COS-squared
    //           3 = другой
    qint16 codeCorrelatedTrace;         // (49-50)   Код коррелированных вибросейсмических трасс:
    //           1 = нет
    //           2 = да
    qint16 binCodeRestoredstHengthening;    // (51-52)   Двоичный код восстановленного усиления:
    //           1 = да
    //           2 = нет
    qint16 codeMethodRestorationAmpl;      // (53-54)    Код метода восстановления амплитуд:
    //           1 = никакой
    //           2 = сферическая дивергенция
    //           3 = АРУ
    //           4 = другой
    qint16 codeMetricSystem;                // (55-56)   Код метрической системы:
    //           1 = метры
    //           2 = футы
    qint16 codePolarityPulseSignal;     // (57-58)   Код полярности импульсного сигнала:
    //           1 = увеличение давления или движение корпуса геофона вверх дают отрицательное число на ленте
    //           2 = увеличение давления или движение корпуса геофона вверх дают положительное число на ленте
    qint16 codePolarityVibrators;          // (59-60)    Код полярности вибраторов:
    //           1 = сейсмический сигнал отстает от пилота на величину от 337.5 до 22.5 градусов
    //           2 = сейсмический сигнал отстает от пилота на величину от 22.5 до 67.5 градусов
    //           3 = сейсмический сигнал отстает от пилота на величину от 67.5 до 112.5 градусов
    //           4 = сейсмический сигнал отстает от пилота на величину от 112.5 до 157.5 градусов
    //           5 = сейсмический сигнал отстает от пилота на величину от 157.5 до 202.5 градусов
    //           6 = сейсмический сигнал отстает от пилота на величину от 202.5 до 247.5 градусов
    //           7 = сейсмический сигнал отстает от пилота на величину от 247.5 до 292.5 градусов
    //           8 = сейсмический сигнал отстает от пилота на величину от 293.5 до 337.5 градусов
    qint8 buffer[340]; // 12 bytes + 12 bytes + 36 bytes + 340 buffer bytes = REEL_HDR_SIZE
};

// структура датчиков формата Sgy
struct StBinarySgystHuct
{
    qint32 sequenceNumberingTraceProf;     // (1-4)  последовательность нумерации трасс на профиле
    qint32 sequenceNumberingTraceCoil;     // (5-8)  последовательность нумерации трасс на катушке
    qint32 numberFieldRecord;              // (9-12)    номер полевой записи
    qint32 numberTraceInsideFieldRecord;   // (13-16)   номер трассы внутри полевой записи
    qint32 numberPointEnergySource;        // (17-20)   номер точки источника энергии
    qint32 numberGroupOGT;                 // (21-24)   номер группы ОГТ
    qint32 numberTraceInsideGroupOGT;      // (25-28)   номер трассы внутри группы ОГТ
    qint16 codeTrace;                      // (29-30)   индентификационный код трассы
    //           1 = сейсмические данные
    //           2 = мертвая
    //           3 = дефектная
    //           4 = отметка момента запуска
    //           5 = вертикальное время
    //           6 = свип
    //           7 = время отсчета
    //           8 = водный срыв
    //           N = дополнительное использование
    qint16 sizeVSumTrace;                  // (31-32)   количество вертикально суммированных трасс
    qint16 sizeHSumTrace;                  // (33-34)   количество горизонтально суммированных трасс
    qint16 usedDataType;                       // (35-36)   использование данных
    //           1 = основные работы
    //           2 = опытные работы
    qint32 distanceSP_groupReceivers;       // (37-40)   расстояние от ПВ до группы приемников (отрицательное, если направление противоположно направлению отстрела профиля)
    qint32 heightGroupSeaLevel;         // (41-44)   высота группы геофонов над уровнем моря (выше уровня моря - положительно)
    qint32 heightSP_SeaLevel;               // (45-48)   высота ПВ над уровнем моря (выше уровня моря - положительно)
    qint32 depthSource;                 // (49-52)   глубина источника (положительна)
    qint32 hLineReductionGroupReceivers;    // (53-56)   высота линии приведения для группы приемников
    qint32 hLineReductionSource;            // (57-60)   высота линии приведения для источника
    qint32 depthWaterSP;                    // (61-64)   шлубина воды для ПВ
    qint32 depthWaterOP;                    // (65-68)   шлубина воды для ПП
    qint16 scaleFactorLines_7;             // (69-70)   масштабный фактор для предыдущих 7 строк со значением плюс или минус 10 в степени 0,1,2,3 или 4 (если положительные значения - умножать, если отрицательные - делить)
    qint16 scaleFactorLines_4;             // (71-72)   масштабный фактор для следующих 4 строк со значением плюс или минус 10 в степени 0,1,2,3 или 4 (если положительные значения - умножать, если отрицательные - делить)
    qint32 XSP;                         // (73-76)   координаты Х для ПВ
    qint32 YSP;                         // (77-80)   координаты У для ПВ
    qint32 XOP;                         // (81-84)   координаты Х для ПП
    qint32 YOP;                         // (85-88)   координаты У для ПП
    qint16 codeUnitsLastLines_4;           // (89-90)   Код единиц координат по предыдущим 4 строкам:
    //           1 = длина (метры или футы)
    //           2 = угловые секунды (в этом случае значения X – долгота, в значения Y - широта, положительные значения означают число секунд от Гринвича или север от Экватора)
    qint16 speedZMS;                       // (91-92)   Скорость ЗМС
    qint16 speedUnderZMS;                  // (93-94)   Скорость под ЗМС
    qint16 vTimeSP;                        // (95-96)   Вертикальное время для ПВ
    qint16 vTimeOP;                        // (97-98)   Вертикальное время для ПП
    qint16 statpopravkaSP;                 // (99-100)  Статпоправка для ПВ
    qint16 statpopravkaOP;                 // (101-102) Статпоправка для ПП
    qint16 generalAppliedStatics;          // (103-104) Общая применяемая статика
    qint16 delayTimeA;                     // (105-106) Время запаздывания A, время в мсек между концом 240-байтного заголовка определения трассы и временным срывом, положительно,
    //           если временной срыв происходит после конца заголовка, временной срыв определяется как импульс инициирования,
    //           который может быть записан на вспомогательную трассу, если не указано иначе
    qint16 delayTimeB;                     // (107-108) Время запаздывания B, время в мсек между временным срывом и временем инициирования источника энергии, может быть положительным или отрицательным
    qint16 delayTimeRecord;                // (109-110) Время задержки записи, время в мсек между временем инициирования источника энергии и временем, когда начинается запись наборов данных (для глубоководных работ, если запись не начинается при времени ноль)
    qint16 timeMtStart;                    // (111-112) Время мьютинга--начало
    qint16 timeMtEnd;                      // (113-114) Время мьютинга--конец
    qint16 numberStepsLineNotSign;         // (115-116) Число шагов в этой трассе (без знака)
    qint16 stepsDigMSNotSign;              // (117-118) Шаг дискретизации; в микросекундах (без знака)
    qint16 codeTypeFieldEquipment;         // (119-120) Код типа усиления полевого оборудования:
    //           1 = фиксированное
    //           2 = бинарное
    //           3 = с плавающей запятой
    //           N = дополнительное использование
    qint16 conststHengthenings;            // (121-122) Константа усиления оборудования
    qint16 stHengtheningEquipment;         // (123-124) Начальное или раннее усиление оборудования
    qint16 correlation;                    // (125-126) Коррелированно:  1 = нет; 2 = да
    qint16 frequencySvipaStart;            // (127-128) Частота свипа в начале
    qint16 frequencySvipaEnd;              // (129-130) Частота свипа в конце
    qint16 durationSvipa;                  // (131-132) Длительность свипа в мсек
    qint16 codeTypeSvipa;                  // (133-134) Код типа свипа:
    //           1 = линейный
    //           2 = квадрат COS
    //           3 = другой
    qint16 durationTraceSvipaStart;        // (135-136) Длительность трассы свипа в начале в мсек
    qint16 durationTraceSvipaEnd;          // (137-138) Длительность трассы свипа в конце в мсек
    qint16 coneType;                       // (139-140) Тип конуса:
    //           1 = линейный
    //           2 = квадрат COS
    //           3 = другой
    qint16 frequencyAlFilterIf;            // (141-142) Частота аляйсингового фильтра, если применялся
    qint16 steepnessAlFilter;              // (143-144) Крутизна алясингового фильтра
    qint16 frequencyRegFilterIf;           // (145-146) Частота режекторного фильтра, если применялся
    qint16 steepnessRegFilter;             // (147-148) Крутизна режекторного фильтра
    qint16 frequencyHfFilterIf;            // (149-150) Частота высокочастотного фильтра, если применялся
    qint16 frequencyLfFilterIf;            // (151-152) Частота низкочастотного фильтра, если применялся
    qint16 steepnessHfFilter;              // (153-154) Крутизна высокочастотного фильтра
    qint16 steepnessLfFilter;              // (155-156) Крутизна низкочастотного фильтра
    qint16 yearRegistHation;               // (157-158) Год регистрации данных
    qint16 dayYear;                        // (159-160) День года
    qint16 hourDay;                        // (161-162) Час дня (24 часа)
    qint16 minutesHour;                    // (163-164) Минуты в часе
    qint16 secondsMinute;                  // (165-166) Секунды в минуте
    qint16 codeTimeZone;                   // (167-168) Код часового пояса:
    //           1 = местное время
    //           2 = по Гринвичу
    //           3 = другой
    qint16 weightMultiplierTrace;          // (169-170) Весовой множитель трассы, определенный как 1/2N Вольт для последнего значащего бита
    qint16 groupNumberGeo;                 // (171-172) Номер группы геофонов в положении один
    qint16 groupNumberGeoFirst;            // (173-174) Номер группы геофонов в первой трассе в оригинальной полевой записи
    qint16 groupNumberGeoEnd;              // (175-176) Номер группы геофонов в последней трассе в оригинальной полевой записи
    qint16 sizeAdmission;                  // (177-178) Размер пропуска (общее число пропущенных групп)
    qint16 codeConeSlip;                   // (179-180) Код конуса проскакивания:
    //           1 = вниз (или назад)
    //           2 = вверх (или вперед)
    qint32 reflect_point_x_coordinate; // изогнутые линии или прямая измерения полного отражения X координата каждой точки (дм).
    qint32 reflect_point_y_coordinate; // изогнутые линии или прямая измерения общей отражения Y координаты каждой точки (дм).
    qint32 reflect_surface_x_coordinate;// координата Х изогнутую линию общие отражающую поверхность каждого из середины (дм).
    qint32 reflect_surface_y_coordinate; // изогнутые линии Y координаты для каждого полного отражения поверхности середине (дм).
    qint32 curveline_export_section_x_coordinate; // координаты Х изогнутый линейный выход (DM) поперечное сечение.
    qint32 curveline_export_section_y_coordinate; // Y координата изогнутый линейный выход (DM) поперечное сечение.
    qint16 measuring_line_receive_pole_number; // рядный пункт приема внутри станции.
    qint16 trace_distance; // сайтов шаг или шаг дорожки (дм).
    qint16 trace_number; // количество дорожек / на ствол.
    qint16 low_speed_zone_speed; // выстрел при низкой скорости со скоростью (м / с).
    qint16 reduction_speed_zone_speed; // падение скорости выстрела со скоростью (м / с).
    qint16 CMP_range_point_distance; // CMP (CDP), или в одной плоскости юаней середина расстояния (дм).
    qint16 measuring_line_range_effect_site_number; // встроенный в общем количестве действительного сайта.
    qint16 section_cmp_point_number; // общая внутреннее поперечное сечение CMP(CDP), точка или совместно бен точка.
    qint16 sp_remain_correcting_value; // снятые остаточные статика (мс).
    qint16 receive_point_correcting_value; // получение точка остаточные статика (мс).
    qint16 all_correcting_value; // общих остаточных статики (мс).
    qint16 low_speed_zone_thickness; // выстрел при низкой скорости, с толщиной (дм).
    qint16 receive_low_speed_zone_thickness; // точка приема на низкой скорости с толщиной (дм).
    qint16 curveline_trace_row_number; // изогнутые линии в номер столбца пути.
    qint16 curveline_trace_col_number; // изогнутые номер строки исполнения.
    qint16 curveline_section_sect_number; // выход изогнутые линии номер сегмента сегмент сечения.
    qint32 curveline_all_trace_number; // встроенный в; int32_t curveline_all_trace_number.
};

//######################################################################################################################################
//#######################################################################################################################################
//######################################################################################################################################

#define HDSGY(stH)\
stH.workNumber >>stH.profileNumber >>stH.coilNumber >>stH.dataTraceNumber >>stH.traceNumber >>\
stH.stepDigMC_Coil >>stH.stepDigMC_record >>stH.numberDigTrace >>stH.numberDigTraceRecord >>\
stH.codeFormatSeismicSets >>stH.frequencyRateOGT >>stH.codeSortingLines >>stH.codeVerticalSum >>\
stH.frequencySvipaStart >>stH.frequencySvipaEnd >>stH.durationSvipa >>stH.typeCodeSvipa >>\
stH.lineNumberChannelSvipa >>stH.durationConeTraceSvipaStart >>stH.durationConeTraceSvipaEnd >>\
stH.codeTypeConeTraceSvipa >>stH.codeCorrelatedTrace >>stH.binCodeRestoredstHengthening >>\
stH.codeMethodRestorationAmpl >>stH.codeMetricSystem >>stH.codePolarityPulseSignal >>stH.codePolarityVibrators

#define HDBINSGY(stH)\
stH.sequenceNumberingTraceProf >>stH.sequenceNumberingTraceCoil >>stH.numberFieldRecord>>\
stH.numberTraceInsideFieldRecord>>stH.numberPointEnergySource >>stH.numberGroupOGT>>\
stH.numberTraceInsideGroupOGT>>stH.codeTrace>>stH.sizeVSumTrace>>stH.sizeHSumTrace>>\
stH.usedDataType>>stH.distanceSP_groupReceivers>>stH.heightGroupSeaLevel>>stH.heightSP_SeaLevel>>\
stH.depthSource>>stH.hLineReductionGroupReceivers>>stH.hLineReductionSource>>stH.depthWaterSP>>\
stH.depthWaterOP>>stH.scaleFactorLines_7>>stH.scaleFactorLines_4>>stH.XSP>>stH.YSP>>stH.XOP>>stH.YOP>>\
stH.codeUnitsLastLines_4>>stH.speedZMS>>stH.speedUnderZMS>>stH.vTimeSP>>stH.vTimeOP>>stH.statpopravkaSP>>\
stH.statpopravkaOP>>stH.generalAppliedStatics>>stH.delayTimeA>>stH.delayTimeB>>stH.delayTimeRecord>>\
stH.timeMtStart>>stH.timeMtEnd>>stH.numberStepsLineNotSign>>stH.stepsDigMSNotSign>>stH.codeTypeFieldEquipment>>\
stH.conststHengthenings>>stH.stHengtheningEquipment>>stH.correlation>>stH.frequencySvipaStart>>\
stH.frequencySvipaEnd>>stH.durationSvipa>>stH.codeTypeSvipa>>stH.durationTraceSvipaStart>>\
stH.durationTraceSvipaEnd>>stH.coneType>>stH.frequencyAlFilterIf>>stH.steepnessAlFilter>>\
stH.frequencyRegFilterIf>>stH.steepnessRegFilter>>stH.frequencyHfFilterIf>>stH.frequencyLfFilterIf>>\
stH.steepnessHfFilter>>stH.steepnessLfFilter>>stH.yearRegistHation>>stH.dayYear>>\
stH.hourDay>>stH.minutesHour>>stH.secondsMinute>>stH.codeTimeZone>>stH.weightMultiplierTrace>>\
stH.groupNumberGeo>>stH.groupNumberGeoFirst>>stH.groupNumberGeoEnd>>stH.sizeAdmission>>\
stH.codeConeSlip>>\
stH.reflect_point_x_coordinate>>stH.reflect_point_y_coordinate>>stH.reflect_surface_x_coordinate>>\
stH.reflect_surface_y_coordinate>>stH.curveline_export_section_x_coordinate>>stH.curveline_export_section_y_coordinate>>\
stH.measuring_line_receive_pole_number>>stH.trace_distance>>stH.trace_number>>stH.low_speed_zone_speed>>\
stH.reduction_speed_zone_speed>>stH.CMP_range_point_distance>>stH.measuring_line_range_effect_site_number>>\
stH.section_cmp_point_number>>stH.sp_remain_correcting_value>>stH.receive_point_correcting_value>>\
stH.all_correcting_value>>stH.low_speed_zone_thickness>>stH.receive_low_speed_zone_thickness>>\
stH.curveline_trace_row_number>>stH.curveline_trace_col_number>>stH.curveline_section_sect_number>>\
stH.curveline_all_trace_number

// структура трассы сейсмограммы
/*!
    \brief структура заголовка трассы сейсмограммы
    Структура включает 80 байт на характеристики  и указателя на формат трассы, описывающие основные характеристии сейсмотрассы.
    Она использукется для определения трасс в классе SeismogramImpl, а также при чтении сейсмограммы
    из формата СЦС3.
*/
struct StTraceHeader
{
    qint32 mg_1;   ///< 1.Логический номер МГ
    qint32 trace;  ///< 2.Номер трассы в полевой МГ
    qint32 mg_2;   ///< 3.Полевой номер МГ
    qint32 actual; ///< 4.Признак актуальности трассы (0 - не акт; 1 - акт)
    qint32 sp;     ///< 5.№ ПВ на профиле
    qint32 xsp;    ///< 6.Координаты ПВ (м)
    qint32 xdp;    ///< 7.Координаты ТГ (м)
    qint32 xop;    ///< 8.Координаты ПП (м)
    qint32 l;      ///< 9.Удаление ПП от ПВ (м)
    uchar form[4];     ///< 10.Формат сейсмических трасс
    qint32 tbeg;   ///< 11.Начальное время общее (мсек)
    qint32 tend;   ///< 12.Конечное время общее (мсек)
    qint32 tbp;    ///< 13.Начальное время полезное (мсек)
    qint32 tep;    ///< 14.Конечное время полезное (мсек)
    qint32 diskr;  ///< 15.Шаг дискретизации (мсек)
    qint32 p;      ///< 16.Вес трассы (%)
    uchar mod[4];      ///< 17.Модификация
    uchar ver[4];      ///< 18.Версия
    qint32 y;      ///< 19.Поперечный вынос ПВ (м)
    qint32 xz;     ///< 20.что-то =)
	DATATYPE* m_pTraceData;   ///< Указатель на начало трассы
};

//ширина строки в профиле
#define SWIDTH(arr) arr.m_iDiscreteCount
//возвращает указатель на doudble для точки
#define GETPOINTER(arr,x,y)\
	(arr.m_ptrData + x + y*SWIDTH(app))
//подготовка цикла по профилю сейсмограммы
#define INITSCAN(arr,th)\
	DATATYPE *ptr, *ptr1, *ptr2; \
	int __x, __y;\
	;

//цикл по профилю сейсмограммы, где значение точки в *ptr
#define RasterScan(arr,th)\
for (__y = 0, ptr = arr.m_ptrData; __y < arr.m_iTraceCount; __y++)\
for (__x = 0; __x < arr.m_iDiscreteCount; __x++,ptr++)

//цикл по двум профилям сейсмограмм, где значение точкек в *ptr1, *ptr2
#define RasterScan2(arr1,arr2,th)\
for (__y = 0, ptr1 = arr1.m_ptrData, ptr2 = arr2.m_ptrData; __y < arr.m_iTraceCount; __y++)\
for (__x = 0; __x < arr.m_iDiscreteCount; __x++, ptr1++, ptr2++)

/*! \class SeismogramImpl
 * \brief Основной класс хранения и обработки сейсмограмм.
 *
 * Основной класс - хранения и обработки сейсмограмм, отвечает за поддержку работы с сейсмограммы внекшних программ.
 * для работы с ним необходимо подключить библиотеку SLib
 * \ingroup Slib
 */

class SeismogramImpl
{
public:
    SeismogramImpl();
    ~SeismogramImpl();

    int m_iTraceCount;      ///< число трасс в профиле
    int m_iDiscreteCount;   ///< максимальное число значений в трассах профиля
    double m_dMaxV;         ///< максимальное значение в трассах;
    double m_dMinV;         ///< минимальное значение в трассах;
    StTraceHeader *m_stSCS3; ///< массив cтрукрур для хранения заголовков трасс;

	DATATYPE *m_ptrData;      ///< массив с трассами;

    /// \fn readSCS5 Функция чтения сейсмограммы из файла CSC3 (СЦС3).
    /// \param sFile_name путь и имя файла в формате scs3 (СЦС3).
    void readSCS3(std::string sFile_name);
	/// \fn readSgy Функция чтения сейсмограммы из файла SEGY (SGY).
	/// \param sFile_name путь и имя файла в формате SEGY (SGY).
	void readSgy(std::string path_file);
	/// \fn readSgy Функция чтения сейсмограммы из файла с вызовом файлового диалога.
	/// \param sFile_name путь и имя файла в формате.
	void read(std::string path_file);
	/// \fn GetPointer Функция указателья индекса в трассе по индексу.
	/// \param x - позиция указателя в трассе.
	/// \param y - номер трассы в профиле.
	DATATYPE *GetPointer(int x, int y) { return (m_ptrData + x + y*m_iDiscreteCount); };
};

#endif