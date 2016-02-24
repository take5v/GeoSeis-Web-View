#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "seismogramimpl.h"

using namespace std;

#define _READ_DATA(type)          \
{\
while (!(file.eof()))\
{\
	type *ptrData, *ptr2; \
	ptrData = new type[nDiscret]; \
	ptr2 = ptrData; \
	scs3h = (stScs3Header*)&(m_stSCS3[it]); \
	stream >> SCS3HEADER(scs3h); \
	nDsc = (m_stSCS3[it].tend - m_stSCS3[it].tbeg) / m_stSCS3[it].diskr; \
	stream.read((char*)ptr2, sizeof(type)*nDsc); \
for (i = 0, ptr1 = ptr, ptr2 = ptrData; i < nDsc; i++, ptr1++, ptr2++)\
	{\
	*ptr1 = (DATATYPE)(*ptr2); \
if (*ptr1> m_dMaxV) m_dMaxV = *ptr1; \
if (*ptr1 < m_dMinV) m_dMinV = *ptr1; \
	}\
	m_stSCS3[it].m_pTraceData = ptr; \
	it++; \
	ptr += nDiscret; \
	delete(ptrData); \
}\
}

float ibm2ieee(const float ibm) {

	// DEFINE AND SETUP UNION VARIABLES
	union value {
		float f;
		unsigned char c[4];
	};
	value src;

	// ASSIGN PARAMETER TO UNION
	src.f = ibm;

	// CONVERT TO FLOAT
	long IntMantissa =
		((long)src.c[1] << 16)
		+ ((long)src.c[2] << 8)
		+ src.c[3];

	float  Mantissa = float(IntMantissa) / float(0x1000000);
	float  PosResult = Mantissa * pow(16.0, double((src.c[0] & 0x7F) - 64));

	if (src.c[0] & 0x80)
		return -PosResult;
	else
		return PosResult;

}

double ibm2ieee(const double ibm) {

	// DEFINE AND SETUP UNION VARIABLES
	union value {
		double f;
		unsigned char c[8];
	};
	value src;

	// ASSIGN PARAMETER TO UNION
	src.f = ibm;

	// CONVERT TO FLOAT
	long long IntMantissa =
		((long long)src.c[1] << 48)
		+ ((long long)src.c[2] << 40)
		+ ((long long)src.c[3] << 32)
		+ ((long long)src.c[4] << 24)
		+ ((long long)src.c[5] << 16)
		+ ((long long)src.c[6] << 8)
		+ src.c[7];

	double  Mantissa = double(IntMantissa) / double(0x1000000);
	double  PosResult = Mantissa * pow(16.0, double((src.c[0] & 0x7F) - 64));

	if (src.c[0] & 0x80)
		return -PosResult;
	else
		return PosResult;

}


#define CHORDERBYTE(c_data) (((c_data << 8) & 0xFF00) | ((c_data >> 8) & 0xFF))

#define CHORDERBYTE4(c_data) ( (c_data << 24)|((c_data << 8) & 0xFF0000)\
	| ((c_data >> 8) & 0xFF00) | ((c_data >> 24) & 0xFF))


SeismogramImpl::SeismogramImpl()
{
	m_iTraceCount = 0; // число трасс в профиле
	m_iDiscreteCount = 0; // максимальное число значений в трассах профиля
	m_dMaxV = 0; // максимальное значение в трассах;
	m_dMinV = 0; // минимальное значение в трассах;
	m_stSCS3 = NULL;  // массив для хранения заголовков трасс
	m_ptrData = NULL; // массив для хранения данных трассам
}

SeismogramImpl::~SeismogramImpl()
{

	//чистим заголовки
	if (m_stSCS3 == NULL)
	{
		delete(m_stSCS3);
		m_stSCS3 = NULL;
	}
	//чистим массив с трассами
	if (m_ptrData == NULL)
	{
		delete(m_ptrData);
		m_ptrData = NULL;
	}
}



// структура трассы сейсмограммы
/*!
	\brief структура заголовка трассы сейсмограммы
	Структура включает 80 байт на характеристики  и указателя на формат трассы, описывающие основные характеристии сейсмотрассы.
	Она использукется для определения трасс в классе SeismogramImpl, а также при чтении сейсмограммы
	из формата СЦС3.
	*/
struct stScs3Header
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
	qint32 form;     ///< 10.Формат сейсмических трасс
	qint32 tbeg;   ///< 11.Начальное время общее (мсек)
	qint32 tend;   ///< 12.Конечное время общее (мсек)
	qint32 tbp;    ///< 13.Начальное время полезное (мсек)
	qint32 tep;    ///< 14.Конечное время полезное (мсек)
	qint32 diskr;  ///< 15.Шаг дискретизации (мсек)
	qint32 p;      ///< 16.Вес трассы (%)
	qint32 mod;      ///< 17.Модификация
	qint32 ver;      ///< 18.Версия
	qint32 y;      ///< 19.Поперечный вынос ПВ (м)
	qint32 xz;     ///< 20.что-то =)
};

#define SCS3HEADER(a) a->mg_1>>a->trace>>a->mg_2>>a->actual\
	>> a->sp >> a->xsp >> a->xdp >> a->xop >> a->l >> a->form >> a->tbeg\
	>> a->tend >> a->tbp >> a->tep >> a->diskr >> a->p >> a->mod >> a->ver\
	>> a->y >> a->xz


void SeismogramImpl::read(string path_file)
{
	//string str = path_file.toStdString();
	//char *path = &str[0];
	//	StTraceHeader stSCS3; // cтрукрура для хранения заголовков трасс;
	//	stScs3Header *scs3h;
	// if (path_file.isEmpty())
	// {
	// 	QFileDialog dialog(NULL);
	// 	dialog.setFileMode(QFileDialog::AnyFile);
	// 	dialog.setNameFilter(QObject::tr("file scs3 (*.scs3 *.scs *.sec *.rec *.);file seg-y (*.sgy *.segy);Any file *.* (*)"));
	// 	dialog.setViewMode(QFileDialog::Detail);
	// 	//if (dialog.exec())
	// 	path_file = dialog.getOpenFileName();// .selectedFiles();

	// }
	// if (path_file.isEmpty()) return;

	// string exp = "";
	// exp = path_file.mid(path_file.lastIndexOf(".") + 1);
	// exp = exp.toLower();

	// if ((exp.compare("segy") == 0) || (exp.compare("sgy") == 0))
	// 	readSgy(path_file);
	// else
	// 	readSCS3(path_file);
	
	readSCS3(path_file);
}


class endianOkFStream : public ifstream
{
public:
    endianOkFStream(const string& filename, ios_base::openmode mode = ios_base::in) :
        ifstream(filename, mode),
        byteOrder(1)
    {
    }
    
    template<typename T>
    endianOkFStream& operator>> (T& val)
    {
        int res;
        this->read((char*)&res, sizeof(T));
        // swap if this is a little endian
        val = byteOrder == 0 ? byteswap(res) : res;
        return *this;
    }

    template<int N>
    void byteswap_array(char (&bytes)[N]) {
        // Optimize this with a platform-specific API as desired.
        for (char *p = bytes, *end = bytes + N - 1; p < end; ++p, --end) {
            char tmp = *p;
            *p = *end;
            *end = tmp;
        }
    }
    
    template<typename T>
    T byteswap(T value) {
        byteswap_array(*reinterpret_cast<char (*)[sizeof(value)]>(&value));
        return value;
    }
    
    void setByteOrder(int newByteOrder)
    {
        byteOrder = newByteOrder;
    }
    
private:
    // 1 is big endian, 0 is little
    int byteOrder;
};



void SeismogramImpl::readSCS3(string path_file)
{
	endianOkFStream file(path_file, ifstream::binary);
    
    StTraceHeader stSCS3; // cтрукрура для хранения заголовков трасс;
    stScs3Header *scs3h;

    
    if (file.is_open())
    {
        endianOkFStream& stream = file;
        scs3h = (stScs3Header*)&stSCS3;

        bool bigEndian = true;
        
        stream >> SCS3HEADER(scs3h);
        
        bool ord = false;
        m_dMaxV = -1.79769e+308;
        m_dMinV = 1.79769e+308;
        
        
        if (scs3h->actual != 0 && scs3h->actual != 1)
        {
            bigEndian = false;
            stream.seekg(0);
            stream.setByteOrder(0);
            stream >> SCS3HEADER(scs3h);
            ord = true;
        }
        
        int nDiscret = (stSCS3.tend - stSCS3.tbeg) / stSCS3.diskr;
        
        stream.seekg(0, ios_base::end);
        int end = stream.tellg();
        stream.seekg(0, ios_base::beg);
        
        ///<шаг 4. определяем тип переменных массива
        int iType; // формат записи трасс
        int step;
        
        if (stSCS3.form[0] == 'i' || stSCS3.form[0] == 'I' || stSCS3.form[3] == 201)
        {
            if (stSCS3.form[1] == '2' || stSCS3.form[2] == 242)
            {
                iType = 1;
                step = nDiscret*sizeof(qint16);
                
            }
            else
                if (stSCS3.form[1] == '4' || stSCS3.form[2] == 244)
                {
                    iType = 2;
                    step = nDiscret*sizeof(qint32);
                    
                }
                else
                    if (stSCS3.form[1] == '8')
                    {
                        iType = 3;
                        step = nDiscret*sizeof(qint64);
                    }
                    else
                    {
                        step = nDiscret*sizeof(qint16);
                        iType = 1;
                    }
        }
        else
        {
            if (stSCS3.form[0] == 'r' || stSCS3.form[0] == 'R' || stSCS3.form[3] == 217)
            {
                if (stSCS3.form[1] == '8')
                {
                    step = nDiscret*sizeof(double);
                    iType = 5;
                }
                else
                {
                    step = nDiscret*sizeof(float);
                    iType = 4;
                }
            }
            else
            {
                step = nDiscret*sizeof(qint16);
                iType = 1;
            }
        }
        
        
        int nTrace = end / (80 + step);
        ///<шаг 2. Резервируется место в памяти под заголовки трасс и массив их хранения.
        m_stSCS3 = new StTraceHeader[nTrace];
        m_ptrData = new DATATYPE[nTrace*nDiscret];
        ///<шаг 3. Заполняется переменные заголовка структуры класса сейсмограммы
        m_iTraceCount = nTrace;      ///< - число трасс в профиле
        m_iDiscreteCount = nDiscret;   ///< - максимальное число значений в трассах
        
        
        
        int it = 0, i;
        int  nDsc;
        DATATYPE *ptr1, *ptr = m_ptrData;
        
        switch (iType)
        {
            case 0: _READ_DATA(qint8);
                break;
            case 1:// _READ_DATA(qint16);
            {
                while (!(file.eof()))
                {
                    qint16 *ptrData, *ptr2, v16;
                    ptrData = new qint16[nDiscret]; ptr2 = ptrData;
                    
                    scs3h = (stScs3Header*)&(m_stSCS3[it]);
                    stream >> SCS3HEADER(scs3h);
                    nDsc = (m_stSCS3[it].tend - m_stSCS3[it].tbeg) / m_stSCS3[it].diskr;
                    stream.read((char*)ptr2, sizeof(qint16)*nDsc);
                    
                    if (it == 1036)
                        it = 1036;
                    if (!ord)
                    {
                        for (i = 0, ptr1 = ptr, ptr2 = ptrData; i < nDsc; i++, ptr1++, ptr2++)
                        {
                            v16 = CHORDERBYTE(*ptr2);
                            *ptr1 = (DATATYPE)v16;
                            if (*ptr1> m_dMaxV) m_dMaxV = *ptr1;
                            if (*ptr1 < m_dMinV) m_dMinV = *ptr1;
                            
                            
                        }
                    }
                    else
                    {
                        for (i = 0, ptr1 = ptr, ptr2 = ptrData; i < nDsc; i++, ptr1++, ptr2++)
                        {
                            *ptr1 = (double)(*ptr2);
                            if (*ptr1> m_dMaxV) m_dMaxV = *ptr1;
                            if (*ptr1 < m_dMinV) m_dMinV = *ptr1;
                        }
                        
                        
                    }
                    m_stSCS3[it].m_pTraceData = ptr;
                    it++;
                    ptr += nDiscret;
                    delete(ptrData);
                }
            }
                break;
            case 2: //_READ_DATA(qint32);
            {
                while (!(file.eof()))
                {
                    qint32 *ptrData, *ptr2;
                    ptrData = new qint32[nDiscret]; ptr2 = ptrData;
                    
                    scs3h = (stScs3Header*)&(m_stSCS3[it]);
                    stream >> SCS3HEADER(scs3h);
                    nDsc = (m_stSCS3[it].tend - m_stSCS3[it].tbeg) / m_stSCS3[it].diskr;
                    stream.read((char*)ptr2, sizeof(qint32)*nDsc);
                    
                    if (!ord)
                    {
                        for (i = 0, ptr1 = ptr, ptr2 = ptrData; i < nDsc; i++, ptr1++, ptr2++)
                        {
                            *ptr1 = (DATATYPE)CHORDERBYTE4(*ptr2);
                            if (*ptr1> m_dMaxV) m_dMaxV = *ptr1;
                            if (*ptr1 < m_dMinV) m_dMinV = *ptr1;
                        }
                        
                    }
                    else
                    {
                        for (i = 0, ptr1 = ptr, ptr2 = ptrData; i < nDsc; i++, ptr1++, ptr2++)
                        {
                            *ptr1 = (DATATYPE)(*ptr2);
                            if (*ptr1> m_dMaxV) m_dMaxV = *ptr1;
                            if (*ptr1 < m_dMinV) m_dMinV = *ptr1;
                        }
                        
                    }
                    
                    m_stSCS3[it].m_pTraceData = ptr;
                    it++;
                    ptr += nDiscret;
                    delete(ptrData);
                }
            }
                break;
            case 3: _READ_DATA(qint64);
                break;
            case 5: //_READ_DATA(float);
            {
                DATATYPE v;
                while (!(file.eof()))
                {
                    DATATYPE *ptrData, *ptr2;
                    ptrData = new DATATYPE[nDiscret]; ptr2 = ptrData;
                    scs3h = (stScs3Header*)&(m_stSCS3[it]);
                    stream >> SCS3HEADER(scs3h);
                    nDsc = (m_stSCS3[it].tend - m_stSCS3[it].tbeg) / m_stSCS3[it].diskr;
                    stream.read((char*)ptr2, sizeof(DATATYPE)*nDsc);
                    if (it == 1036)
                        it = 1036;
                    if (!ord)
                    {
                        for (i = 0, ptr1 = ptr, ptr2 = ptrData; i < nDsc; i++, ptr1++, ptr2++)
                        {
                            
                            v = ibm2ieee(*ptr2);
                            *ptr1 = (DATATYPE)v;
                            if (*ptr1> m_dMaxV) m_dMaxV = *ptr1;
                            if (*ptr1 < m_dMinV) m_dMinV = *ptr1;
                        }
                    }
                    else
                    {
                        for (i = 0, ptr1 = ptr, ptr2 = ptrData; i < nDsc; i++, ptr1++, ptr2++)
                        {
                            *ptr1 = (DATATYPE)(*ptr2);
                            if (*ptr1> m_dMaxV) m_dMaxV = *ptr1;
                            if (*ptr1 < m_dMinV) m_dMinV = *ptr1;
                        }
                        
                    }
                    m_stSCS3[it].m_pTraceData = ptr;
                    it++;
                    ptr += nDiscret;
                    delete(ptrData);
                }
            }
                break;
            case 4:
                DATATYPE *ptrData = m_ptrData, *ptr2;
                while (!file.eof() && it < nTrace)
                {
                    file.read((char*)&(m_stSCS3[it]), 80);
                    int  nDsc = (m_stSCS3[it].tend - m_stSCS3[it].tbeg) / m_stSCS3[it].diskr;
                    file.read((char*)ptrData, sizeof(DATATYPE)*nDsc);
                    for (i = 0, ptr2 = ptrData; i < nDsc; i++, ptr2++)
                    {
                        if (*ptr2> m_dMaxV) m_dMaxV = *ptr2;
                        if (*ptr2 < m_dMinV) m_dMinV = *ptr2;
                    }
                }
                break;
                
                
        }
    }
    
    file.close();
}



void SeismogramImpl::readSgy(string path_file)
{
	// QFile file(path_file);
	// int pos_file = 0, blen = 0;
	// if (file.exists())
	// 	qDebug("Opening File ... ");
	// else
	// {
	// 	qDebug("File not found.");
	// 	return;
	// }
	// // DATATYPE
	// float *ptr;
	// int x, xmax, y, ymax;
	// //объявляем массивы для чтения заголовков
	// char  _EbcdicHeader[3200];
	// //char  _BinaryHeader[400];
	// //char  _TraceHeader[240];
	// //char  _INPTRC[10000];
	// //float _Data[10000];
	// int sp;

	// // структуры
	// StHeadingSgystHuct myHSgyStruct;
	// StBinarySgystHuct mySgyStruct;



	// QDataStream stream(&file);
	// stream.setVersion(QDataStream::Qt_4_5);
	// //	stream.setByteOrder(QDataStream::LittleEndian);

	// stream.setByteOrder(QDataStream::BigEndian);
	// if (file.open(QIODevice::ReadOnly))
	// {
	// 	//for (int i = 0; i < 3200;i++)
	// 	stream.readRawData(_EbcdicHeader, 3200); ///<шаг 1. Пропускается 3200 байт, которые отводятся под текстовый заголовок
	// 	//		file.seek(3200);
	// 	pos_file = file.pos();
	// 	//file.read((&myHSgyStruct, 60);
	// 	stream >> HDSGY(myHSgyStruct); ///<шаг 2. Читается бинарный заголовок профиля
	// 	pos_file = file.pos();
	// 	file.seek(3600);
	// 	sp = file.pos();
	// 	ymax = m_iTraceCount = myHSgyStruct.dataTraceNumber; ///<шаг 3. Определяется характеристики профиля: количество трасс,
	// 	xmax = m_iDiscreteCount = myHSgyStruct.numberDigTrace; /// число дискретов в трассе,

	// 	sp = myHSgyStruct.stepDigMC_Coil; /// шаг 3. Определяются параметры дискретизации в трассе.

	// 	m_ptrData = new DATATYPE[xmax*ymax];
	// 	ptr = m_ptrData;
	// 	m_stSCS3 = new StTraceHeader[m_iTraceCount];
	// 	//stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
	// 	//stream.setFloatingPointPrecision(QDataStream::FloatingPointPrecision);
	// 	m_dMaxV = -1.79769e+308;
	// 	m_dMinV = 1.79769e+308;
	// 	sp = file.pos();
	// 	//		if (myHSgyStruct.codeFormatSeismicSets == 1)
	// 	//		{
	// 	//		}



	// 	for (y = 0; y < ymax; y++)  ///<шаг 4. В цикле считываются и заполняются характеристики трасс
	// 	{
	// 		pos_file = file.pos();
	// 		stream >> HDBINSGY(mySgyStruct); ///<шаг 4.1 Считывается Бинарная структура описания трассы
	// 		sp = file.pos();

	// 		m_stSCS3[y].actual = 1;				 ///<шаг 4.2 заполняются структуры описания трассы:

	// 		m_stSCS3[y].mg_1 = mySgyStruct.numberFieldRecord;   ///< 1.Логический номер МГ
	// 		m_stSCS3[y].trace = mySgyStruct.numberTraceInsideFieldRecord;  ///< 2.Номер трассы в полевой МГ
	// 		m_stSCS3[y].mg_2 = mySgyStruct.numberFieldRecord;   ///< 3.Полевой номер МГ
	// 		m_stSCS3[y].actual = 1; ///< 4.Признак актуальности трассы (0 - не акт; 1 - акт)
	// 		m_stSCS3[y].sp = mySgyStruct.numberFieldRecord;     ///< 5.№ ПВ на профиле
	// 		m_stSCS3[y].xsp = mySgyStruct.XSP;    ///< 6.Координаты ПВ (м)
	// 		m_stSCS3[y].xdp = mySgyStruct.depthSource;    ///< 7.Координаты ТГ (м)
	// 		m_stSCS3[y].xop = mySgyStruct.XOP;    ///< 8.Координаты ПП (м)
	// 		m_stSCS3[y].l = abs(mySgyStruct.XSP - mySgyStruct.XOP);      ///< 9.Удаление ПП от ПВ (м)
	// 		//			m_stSCS3[y].form[4];     ///< 10.Формат сейсмических трасс
	// 		m_stSCS3[y].tbeg = 0;   ///< 11.Начальное время общее (мсек)
	// 		m_stSCS3[y].tend = (mySgyStruct.numberStepsLineNotSign - 1)*mySgyStruct.stepsDigMSNotSign;   ///< 12.Конечное время общее (мсек)
	// 		m_stSCS3[y].tbp = mySgyStruct.vTimeSP;    ///< 13.Начальное время полезное (мсек)
	// 		m_stSCS3[y].tep = mySgyStruct.vTimeOP;    ///< 14.Конечное время полезное (мсек)
	// 		m_stSCS3[y].diskr = mySgyStruct.stepsDigMSNotSign;  ///< 15.Шаг дискретизации (мсек)
	// 		m_stSCS3[y].p = mySgyStruct.weightMultiplierTrace;      ///< 16.Вес трассы (%)
	// 		//			m_stSCS3[y].mod[4];      ///< 17.Модификация
	// 		//			m_stSCS3[y].ver[4];      ///< 18.Версия
	// 		m_stSCS3[y].y = mySgyStruct.YSP;      ///< 19.Поперечный вынос ПВ (м)
	// 		m_stSCS3[y].xz = mySgyStruct.YOP;     ///< 20.Поперечный вынос ПП (м)
	// 		m_stSCS3[y].m_pTraceData = ptr;   ///< Указатель на начало трассы
	// 		//stream.readRawData((char*)ptr,xmax);
	// 		DATATYPE val;
	// 		//stream.setFloatingPointPrecision(QDataStream::DoublePrecision);// :SinglePrecision);
	// 		//			stream.setByteOrder(QDataStream::LittleEndian);
	// 		//stream.setByteOrder(QDataStream::BigEndian);



	// 		///<шаг 4.3 В считываются значения трасс для заданных типов:
	// 		if (myHSgyStruct.codeFormatSeismicSets == 1)
	// 		{
	// 			///		- 1 = IBM плавающая запятая (4 байта)
	// 			//    blen=stream.readRawData((char*)ptr, sizeof(DATATYPE)*(uint)xmax);
	// 			file.read((char*)ptr, sizeof(DATATYPE)*xmax);
	// 			//  ptr = m_stSCS3[y].m_pTraceData;
	// 			for (x = 0; x < xmax; x++, ptr++)
	// 			{
	// 				//	stream >>(DATATYPE)*ptr;
	// 				if (*ptr)
	// 					val = ibm2ieee(*ptr);
	// 				*ptr = val;
	// 				if (*ptr> m_dMaxV) m_dMaxV = *ptr;
	// 				if (*ptr < m_dMinV) m_dMinV = *ptr;

	// 			}
	// 		}
	// 		else
	// 		if (myHSgyStruct.codeFormatSeismicSets == 5)
	// 		{
	// 			///		- 5 = IEEE плавающая запятая (4 байта)
	// 			float val;
	// 			ptr = m_stSCS3[y].m_pTraceData;
	// 			for (x = 0; x < xmax; x++, ptr++)
	// 			{
	// 				stream >> val;
	// 				(*ptr) = val;
	// 				if (*ptr> m_dMaxV) m_dMaxV = *ptr;
	// 				if (*ptr < m_dMinV) m_dMinV = *ptr;

	// 			}
	// 		}
	// 		else
	// 		if (myHSgyStruct.codeFormatSeismicSets == 2)
	// 		{
	// 			///		- 2 = целое значение (4 байта)
	// 			qint32 val;
	// 			ptr = m_stSCS3[y].m_pTraceData;
	// 			for (x = 0; x < xmax; x++, ptr++)
	// 			{
	// 				stream >> val;
	// 				*ptr = val;
	// 				if (*ptr> m_dMaxV) m_dMaxV = *ptr;
	// 				if (*ptr < m_dMinV) m_dMinV = *ptr;

	// 			}
	// 		}
	// 		else
	// 		if (myHSgyStruct.codeFormatSeismicSets == 3)
	// 		{
	// 			///		- 3 = целое значение (2 байта)
	// 			qint16 val;
	// 			ptr = m_stSCS3[y].m_pTraceData;
	// 			for (x = 0; x < xmax; x++, ptr++)
	// 			{
	// 				stream >> val;
	// 				*ptr = val;
	// 				if (*ptr> m_dMaxV) m_dMaxV = *ptr;
	// 				if (*ptr < m_dMinV) m_dMinV = *ptr;

	// 			}
	// 		}
	// 		else
	// 		if (myHSgyStruct.codeFormatSeismicSets == 0)
	// 		{
	// 			///		- 0 = Беззнаковое целое (1 байт), используется для отображения на экране
	// 			uchar val;
	// 			ptr = m_stSCS3[y].m_pTraceData;
	// 			for (x = 0; x < xmax; x++, ptr++)
	// 			{
	// 				stream >> val;
	// 				*ptr = val;
	// 				if (*ptr> m_dMaxV) m_dMaxV = *ptr;
	// 				if (*ptr < m_dMinV) m_dMinV = *ptr;

	// 			}
	// 		}
	// 	}
	// }
}


