
#include "guitextlib.h"

const std::map<unsigned int, std::wstring> EMRE_MSG_SHORT_RUS = {
	{EMRE_BACKGROUNG_IR, L"Фоновая ИК засветка"},
	{EMRE_NO_PUPILS, L"Зрачки не обнаружены"},
	{EMRE_NO_FOCUS, L"Изображение вне фокуса"},
	{EMRE_NO_EYEFIX, L"Нет фиксации взгляда"},
	{EMRE_DIM_REFLEX, L"Рефлекс не яркий"},
	{EMRE_PUPIL_TOO_SMALL, L"Слишком маленький зрачок"},
	{EMRE_PUPIL_TOO_BIG, L"Слишком большой зрачок"},
	{EMRE_EYELID, L"Веко перекрывает зрачок"},
	{EMRE_EYELASH, L"Ресница в области зрачка"},
	{EMRE_OTHER, L"Измерения не полные"},
	{EMRE_MONOCULAR, L"Монокулярное измерение"}
};

const std::wstring EMRE_MSG_SHORT_RUS_OK = L"Измерение успешно";

const std::map<unsigned int, std::wstring> EMRE_MSG_DESC_RUS = {
	{EMRE_BACKGROUNG_IR, L"Затемните помещение"},
	{EMRE_NO_PUPILS, L"Направьте прибор на пациента"},
	{EMRE_NO_FOCUS, L"Отрегулируйте дальность"},
	{EMRE_NO_EYEFIX, L"Необходимо смотреть в камеру"},
	{EMRE_DIM_REFLEX, L"Дефект глаз или нет фокуса"},
	{EMRE_PUPIL_TOO_SMALL, L"Затемните помещение"},
	{EMRE_PUPIL_TOO_BIG, L"Отрегулируйте яркость"},
	{EMRE_EYELID, L"Расширьте глаза"},
	{EMRE_EYELASH, L"Сбрейте ресницы"},
	{EMRE_OTHER, L"Возникла иная ошибка"},
	{EMRE_MONOCULAR, L"Закройте второй глаз"}
};

const std::wstring EMRE_MSG_DESC_RUS_OK = L"Так держать!";

bool eyeMeasResultErrorText(unsigned int error_word, unsigned int language, std::wstring & error_text, std::wstring & error_description)
{
	if (language >= _EYEGUILANG_MAX_) return false;
	for (unsigned int i_bit = 0; i_bit < (sizeof(error_word)*8); i_bit++) {
		if ((error_word >> i_bit)&0x1) {
			if (EMRE_MSG_SHORT_RUS.count(i_bit) > 0) {
				error_text = EMRE_MSG_SHORT_RUS.at(i_bit);
				error_description = EMRE_MSG_DESC_RUS.at(i_bit);
				return true;
			} else return false;
		}
	}
	error_text = EMRE_MSG_SHORT_RUS_OK;
	error_description = EMRE_MSG_DESC_RUS_OK;
	return true;
}
