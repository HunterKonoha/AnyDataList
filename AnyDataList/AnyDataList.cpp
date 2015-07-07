#include "AnyDataList.h"
#include "boost\algorithm\string.hpp"
#include "boost\dynamic_bitset.hpp"
#include "boost\lexical_cast.hpp"

AnyDataList::AnyDataList(std::string e) {
	//�܂��ȒP�ɏ����������Ă��邩�ǂ����m�F����B
	//NOTE:���݂̎d�l�ł́A[]�̑O��ɕ����������Ă��G���[�͏o�Ȃ��B

	//�f�[�^�̎n�܂�('{')�ƏI���('}')�̈ʒu�B
	auto datastart = this->find(e, '{');
	std::string::size_type dataend;
	if (datastart == std::string::npos) {
		//exception(���@�G���[)
		//{��������Ȃ��������Ƃ������B
		s3d::MessageBox::Show(L"���@�G���[�ł��B\n{��������܂���ł����B");
		return;
	}
	if (datastart > this->find(e, '}')) {
		//exception(���@�G���[)
		//{�̑O��}�������������Ƃ�����
		s3d::MessageBox::Show(L"���@�G���[�ł��B\n{�̑O��}��������܂����B");
		return;
	}
	//{}�̐��̈�v��n�����ʂƃl�X�g��}�̌�̕�������m�F����B
	{
		//braces[0]��'{'�̈ʒu�������Bbraces[1]��'}'�̈ʒu�������B
		std::array<std::vector<std::string::size_type>, 2> braces;
		for (auto pos = datastart; pos != std::string::npos; pos = this->find(e, '{', braces[0][braces[0].size() - 1] + 1)) {
			braces[0].push_back(pos);
		}
		for (auto pos = this->find(e, '}', datastart); pos != std::string::npos; pos = this->find(e, '}', braces[1][braces[1].size() - 1] + 1)) {
			braces[1].push_back(pos);
		}
		dataend = braces[1][braces[1].size() - 1];
		if (braces[0].size() != braces[1].size()) {
			//exception(���@�G���[)
			//{��}�̐�����v���Ă��Ȃ����Ƃ�����
			s3d::MessageBox::Show(L"���@�G���[�ł��B{��}�̐�����v���܂���ł����B");
			return;
		}
		//�f�[�^��n�����ʂ����o����
		{
			//connect[0]�͎n�܂�̊���({)����ǂꂾ�����ʂ������Ă��邩�Aconnect[1]��connect[0]�̏I��芇��(})��
			std::array<int, 2> connect{ { 0, 0 } };
			for (int i = datastart; i < e.size(); ++i) {
				if (e[i] == '{') ++connect[0];
				else break;
			}
			for (int i = dataend; i >= 0; --i) {
				if (e[i] == '}')++connect[1];
				else {
					for (; i >= 0; --i) {
						if (e[i] == ';') {
							--connect[1];
							break;
						}
						else if (!this->IsSkipChar(e[i]))break;
					}
					break;
				}
			}
			if (connect[0]>1 || connect[1] > 1) {
				if (connect[0] == connect[1]) {
					//exception(���@�G���[)
					//n�����ʂɂȂ��Ă��邱�Ƃ������B
					s3d::MessageBox::Show(s3d::Widen("���@�G���[�ł��B\n" + boost::lexical_cast<std::string>(connect[0]) + "�����ʂɂȂ��Ă��܂��B"));
					return;
				}
				else if (connect[0]<connect[1]) {
					//exception(���@�G���[)
					//�Ō�̍��̊֐���`��;��Y��Ă��邩�I���̊���}�͑������Ƃ������B
					s3d::MessageBox::Show(L"���@�G���[�ł��B\n�I���̊���(})���������o����܂����B\n�֐���`��;��Y��Ă��邩�A���������Ă��܂��B");
					return;
				}
				else {
					//exception(���@�G���[)
					//�͂��߂�{���������Ƃ�����
					s3d::MessageBox::Show(L"���@�G���[�ł��B�n�܂�̊���({)���������o����܂����B");
					return;
				}
			}
		}
		//{}�̃l�X�g�������������o����B
		{
			std::array<boost::dynamic_bitset<>, 2> serchbrance{ { boost::dynamic_bitset < >(braces[0].size()), boost::dynamic_bitset < >(braces[1].size()) } };
			std::array<int, 2> bracepos{ { 0, 0 } };
			auto func = [](const boost::dynamic_bitset<>& List, int Pos){
				for (int i = Pos + 1; i < List.size(); ++i) {
					if (!List[i])return i - Pos;
				}
				return 0;
			};
			while (true) {
				if (func(serchbrance[0], bracepos[0]) != 0 && braces[0][bracepos[0] + func(serchbrance[0], bracepos[0])] < braces[1][bracepos[1]]) {
					do {
						++bracepos[0];
					} while (bracepos[0] < braces[0].size() && serchbrance[0][bracepos[0]]);
				}
				else {
					serchbrance[0][bracepos[0]] = true;
					serchbrance[1][bracepos[1]] = true;
					++bracepos[1];
					do {
						--bracepos[0];
					} while (bracepos[0] > 0 && serchbrance[0][bracepos[0]]);
				}
				if (serchbrance[0].count() == serchbrance[0].size() && serchbrance[1].count() == serchbrance[1].size()) {
					break;
				}
				if (serchbrance[0][0] && func(serchbrance[0], 0) != 0) {
					//exception(���@�G���[)
					//{}�̃l�X�g�������������Ƃ�����
					s3d::MessageBox::Show(L"���@�G���[�ł��B{}�̃y�A����v���܂���ł����B");
					return;
				}
			}
		}
		for (int i = braces[1][braces[1].size() - 1] + 1; i < e.size(); ++i) {
			if (!this->IsSkipChar(e[i])) {
				if (e[i] == '/') {
					i = this->GetNextNewLineSign(e, i);
				}
				else {
					//exception(���@�G���[)
					//}�̌�ɕ��������������Ƃ�����
					s3d::MessageBox::Show(L"���@�G���[�ł��B}�̌�ɕs���ȕ��������o���܂����B");
					return;
				}
			}
		}
	}
	//���O�̎w�肪����Ζ��O������B���O��[]{}��[]�̒��g�ɏ������B
	std::string name("");
	{
		auto selfnamecode = std::string(e.begin(), e.begin() + datastart);
		size_t pos[2] = { selfnamecode.find("["), selfnamecode.find("]") };
		if ((pos[0] != std::string::npos) ^ (pos[1] != std::string::npos)) {
			//exception(���@�G���[)
			//[��]�̂ǂ��炩��t���Y��̎��ɂ��̏��ɓ���B
			s3d::MessageBox::Show(L"���@�G���[�ł��B\n[��]�̕t���Y��ł��B");
			return;
		}
		if (pos[0] > pos[1]) {
			//exception(���@�G���[)
			//[��]���������������ɂ��̒��ɓ���
			s3d::MessageBox::Show(L"���@�G���[�ł��B\n[��]��葁�����Ă��܂��B");
			return;
		}
		else if (pos[0] != std::string::npos&&pos[1] != std::string::npos) {
			//���̒��ɓ���������[]�Ə��Ԓʂ�ɂȂ��Ă�͂�
			//�󔒂ƃ^�u�Ɖ��s�͖�������
			auto value = std::string(e.begin() + pos[0] + 1, e.begin() + pos[1]);
			for (int i = 0; i < value.size(); ++i) {
				if (!this->IsSkipChar(value[i])) {
					if (value[i] == '/') {
						i = this->GetNextNewLineSign(e, i);
					}
					else {
						name += value[i];
					}
				}
			}
		}
	}
	//���O��t���I�������{}���̉��
	//�ŏ���std::vector<std::vector<std::string>>([0]���ϐ�����[1]���l)���\�z����
	std::vector<std::vector<std::string>> valuedata;
	{
		//�ϐ���=�l�����o���B���̎��_�ł̓S�~�������Ă�̂ŃS�~�������B
		for (auto pos = this->find(e, ',', datastart), oldpos = datastart;; oldpos = pos, pos = this->find(e, ',', pos + 1, { std::array < char, 2 > {{ '(', ')' }}, std::array < char, 2 > {{ '{', '}' }} })) {
			std::string value;
			{
				//�Ō�̑g(,��������Ȃ��Ƃ���)�ɗ�����A}�ȍ~�̃S�~�����Ă�̂Ŕ͈͎w���ύX����B
				auto t_value = std::string(e.begin() + oldpos + 1, (pos == std::string::npos ? e.begin() + dataend : e.begin() + pos));
				//��,�^�u,���s�R�[�h,�R�����g���폜�B
				for (int i = 0; i < t_value.size(); ++i) {
					if (!this->IsSkipChar(t_value[i])) {
						if (t_value[i] == '/')i = this->GetNextNewLineSign(t_value, i);
						else value += t_value[i];
					}
				}
			}
			auto equal = value.find('=');
			if (equal == std::string::npos) {
				//exception(���@�G���[)
				//�ϐ��������w�肳��Ă��Ȃ�(=���Ȃ�)���Ƃ������B
				s3d::MessageBox::Show(L"���@�G���[�ł��B\n'='������܂���B");
				return;
			}
			if (equal == value.size() - 1) {
				//exception(���@�G���[)
				//�ϐ��̒l���Ȃ����Ƃ������B
				s3d::MessageBox::Show(L"���@�G���[�ł��B\n�ϐ��̒l������܂���B");
				return;
			}
			//=�̉E����AnyDataList�Ɗ֐��̍\��({})���Ȃ��ꍇ�̂�=��G���[���o���B
			//{�ł������肵�Ă��Ȃ��̂́A}�����������ꍇ�����ŃG���[���o�����ċA�I�ɌĂяo����AnyDataList��{��}�̐�����v���Ă��Ȃ��G���[�̂ق����킩��₷������
			//�֐��̏ꍇ��if���Ƃ���������������(����)
			if (value.find('{', equal + 1) == std::string::npos&&value.find('=', equal + 1) != std::string::npos) {
				//exception(���@�G���[)
				//=����ȏ゠�邱�Ƃ������B
				s3d::MessageBox::Show(L"���@�G���[�ł��B\n'='����ȏ㔭�����܂����B");
				return;
			}
			valuedata.push_back({ std::string(value.begin(), value.begin() + equal), std::string(value.begin() + equal + 1, value.end()) });
			if (pos == std::string::npos)break;
		}
	}
	//��͂��I������炻�ꂼ���boost::any�ɕϊ����Ă���
}
std::string::size_type AnyDataList::find(const std::string& String, const char& SearchCode, std::string::size_type StartPos, std::vector<std::array<char, 2>> SkipChar) {
	for (;; ++StartPos) {
		if (StartPos == String.size()) {
			return std::string::npos;
		}
		//�R�����g(//)�̏����B���s�R�[�h��������܂�StartPos��i�߂�B���s�R�[�h��������Ȃ�������A�T���Ă��镶���͂Ȃ��̂�npos��Ԃ��B
		if (String[StartPos] == '/' && StartPos + 1 < String.size() && String[StartPos + 1] == '/') {
			StartPos = this->GetNextNewLineSign(String, StartPos);
			if (StartPos == std::string::npos)return std::string::npos;
		}
		//������("*")�̏����B"���������玟��"��������܂�StartPos��i�߂�B
		else if (String[StartPos] == '\"') {
			StartPos = String.find('\"', StartPos + 1);
			if (StartPos == std::string::npos) {
				//exception(���@�G���[)
				//"�̃y�A��������Ȃ����Ƃ�����
				s3d::MessageBox::Show(L"���@�G���[�ł��B\n\"�̃y�A�������邱�Ƃ��ł��܂���ł���");
				return std::string::npos;
			}
		}
		//�Ăє�΂��������΂�
		else {
			bool c = false;
			for (auto& t_skippair : SkipChar) {
				if (String[StartPos] == t_skippair[0]) {
					StartPos = this->find(String, t_skippair[1], StartPos + 1, SkipChar);
					if (StartPos == std::string::npos) {
						//exception(���@�G���[)
						//�ǂݔ�΂������̃y�A��������Ȃ���������
						s3d::MessageBox::Show(s3d::Widen(std::string("���@�G���[�ł��B\n�y�A�������邱�Ƃ��ł��܂���ł���")));
						return std::string::npos;
					}
					c = true;
					break;
				}
			}
			if (!c&&String[StartPos] == SearchCode) {
				return StartPos;
			}
		}
		//else if (String[StartPos] == SearchCode)return StartPos;
	}
}
bool AnyDataList::IsSkipChar(const char&Char) {
	switch (Char) {
		//��(���p�̂�),�^�u���s�R�[�h(\n,\r)�̓X�L�b�v����B
		case ' ':
		case '\t':
		case	'\n':
		case '\r':
			return true;
			break;
		default:
			return false;
			break;
	}
}
void AnyDataList::PreInit() {

}
std::string::size_type AnyDataList::GetNextNewLineSign(const std::string& String, std::string::size_type StartPos) {
	//���s�R�[�h�܂œǂݍ��ނ��A���s�R�[�h��2�o�C�g�������ꍇ�͂��̕���������B
	auto result = String.find('\n', StartPos);
	if (result == std::string::npos)return std::string::npos;
	if (result + 1 < String.size() && String[result + 1] == '\r')return result + 1;
	else return result;
}