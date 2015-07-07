#pragma once
#include <string>
#include <vector>
#include <array>

class AnyDataList {
	private:

	private:
		void PreInit();
		/// <summary>
		/// �w�肳�ꂽ�ʒu����ŏ��Ɍ����������s�R�[�h(\n,\n\r)�̏I���̈ʒu��Ԃ��܂��B
		/// </summary>
		/// <param name="String">�������镶����</param>
		/// <param name="StartPos">�����ʒu</param>
		/// <returns>���s�R�[�h(\n,\n\r)�̏I���̈ʒu</returns>
		std::string::size_type GetNextNewLineSign(const std::string& String, std::string::size_type StartPos = 0);

	public:
		AnyDataList(std::string e);
		~AnyDataList() {};
		/// <summary>
		/// �R�����g��""���l�����Ȃ��當������w�肵���ʒu���猟�����āA�ŏ��Ɏw�肵���������������ꏊ��Ԃ��B
		/// </summary>
		/// <param name="String">�������镶����</param>
		/// <param name="SearchCode">�w�肷�镶��</param>
		/// <param name="StartPos">�ŏ��̈ʒu</param>
		/// <param name="SkipChar">�ǉ��̓ǂݔ�΂������̃y�A</param>
		/// <returns>�ŏ��ɕ����񂪂������ꏊ</returns>
		std::string::size_type find(const std::string& String, const char& SearchCode, std::string::size_type StartPos = 0, std::vector<std::array<char, 2>> SkipChar = {});
		/// <summary>
		/// �������X�L�b�v(����)����镶�����ǂ������肵�܂��B
		/// </summary>
		/// <param name="Char">���肷�镶��</param>
		/// <returns>�X�L�b�v(����)���邩�ǂ���</returns>
		bool IsSkipChar(const char& Char);
};