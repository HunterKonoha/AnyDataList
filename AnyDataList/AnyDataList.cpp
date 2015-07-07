#include "AnyDataList.h"
#include "boost\algorithm\string.hpp"
#include "boost\dynamic_bitset.hpp"
#include "boost\lexical_cast.hpp"

AnyDataList::AnyDataList(std::string e) {
	//まず簡単に書式が合っているかどうか確認する。
	//NOTE:現在の仕様では、[]の前後に文字があってもエラーは出ない。

	//データの始まり('{')と終わり('}')の位置。
	auto datastart = this->find(e, '{');
	std::string::size_type dataend;
	if (datastart == std::string::npos) {
		//exception(文法エラー)
		//{が見つからなかったことを示す。
		s3d::MessageBox::Show(L"文法エラーです。\n{が見つかりませんでした。");
		return;
	}
	if (datastart > this->find(e, '}')) {
		//exception(文法エラー)
		//{の前に}が見つかったことを示す
		s3d::MessageBox::Show(L"文法エラーです。\n{の前に}が見つかりました。");
		return;
	}
	//{}の数の一致とn次括弧とネストと}の後の文字列を確認する。
	{
		//braces[0]は'{'の位置を示す。braces[1]は'}'の位置を示す。
		std::array<std::vector<std::string::size_type>, 2> braces;
		for (auto pos = datastart; pos != std::string::npos; pos = this->find(e, '{', braces[0][braces[0].size() - 1] + 1)) {
			braces[0].push_back(pos);
		}
		for (auto pos = this->find(e, '}', datastart); pos != std::string::npos; pos = this->find(e, '}', braces[1][braces[1].size() - 1] + 1)) {
			braces[1].push_back(pos);
		}
		dataend = braces[1][braces[1].size() - 1];
		if (braces[0].size() != braces[1].size()) {
			//exception(文法エラー)
			//{と}の数が一致していないことを示す
			s3d::MessageBox::Show(L"文法エラーです。{と}の数が一致しませんでした。");
			return;
		}
		//データのn次括弧を検出する
		{
			//connect[0]は始まりの括弧({)からどれだけ括弧が続いているか、connect[1]はconnect[0]の終わり括弧(})版
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
					//exception(文法エラー)
					//n次括弧になっていることを示す。
					s3d::MessageBox::Show(s3d::Widen("文法エラーです。\n" + boost::lexical_cast<std::string>(connect[0]) + "次括弧になっています。"));
					return;
				}
				else if (connect[0]<connect[1]) {
					//exception(文法エラー)
					//最後の項の関数定義で;を忘れているか終わりの括弧}は多いことを示す。
					s3d::MessageBox::Show(L"文法エラーです。\n終わりの括弧(})が多く検出されました。\n関数定義で;を忘れているか、多く書いています。");
					return;
				}
				else {
					//exception(文法エラー)
					//はじめの{が多いことを示す
					s3d::MessageBox::Show(L"文法エラーです。始まりの括弧({)が多く検出されました。");
					return;
				}
			}
		}
		//{}のネストが正しいか検出する。
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
					//exception(文法エラー)
					//{}のネストがおかしいことを示す
					s3d::MessageBox::Show(L"文法エラーです。{}のペアが一致しませんでした。");
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
					//exception(文法エラー)
					//}の後に文字があったことを示す
					s3d::MessageBox::Show(L"文法エラーです。}の後に不正な文字を検出しました。");
					return;
				}
			}
		}
	}
	//名前の指定があれば名前をつける。名前は[]{}の[]の中身に書かれる。
	std::string name("");
	{
		auto selfnamecode = std::string(e.begin(), e.begin() + datastart);
		size_t pos[2] = { selfnamecode.find("["), selfnamecode.find("]") };
		if ((pos[0] != std::string::npos) ^ (pos[1] != std::string::npos)) {
			//exception(文法エラー)
			//[か]のどちらかを付け忘れの時にこの所に入る。
			s3d::MessageBox::Show(L"文法エラーです。\n[か]の付け忘れです。");
			return;
		}
		if (pos[0] > pos[1]) {
			//exception(文法エラー)
			//[が]よりも早く来た時にこの中に入る
			s3d::MessageBox::Show(L"文法エラーです。\n[か]より早く来ています。");
			return;
		}
		else if (pos[0] != std::string::npos&&pos[1] != std::string::npos) {
			//この中に入った時は[]と順番通りになってるはず
			//空白とタブと改行は無視する
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
	//名前を付け終わったら{}内の解析
	//最初にstd::vector<std::vector<std::string>>([0]が変数名で[1]が値)を構築する
	std::vector<std::vector<std::string>> valuedata;
	{
		//変数名=値を取り出す。この時点ではゴミが入ってるのでゴミを消す。
		for (auto pos = this->find(e, ',', datastart), oldpos = datastart;; oldpos = pos, pos = this->find(e, ',', pos + 1, { std::array < char, 2 > {{ '(', ')' }}, std::array < char, 2 > {{ '{', '}' }} })) {
			std::string value;
			{
				//最後の組(,が見つからないところ)に来たら、}以降のゴミもついてるので範囲指定を変更する。
				auto t_value = std::string(e.begin() + oldpos + 1, (pos == std::string::npos ? e.begin() + dataend : e.begin() + pos));
				//空白,タブ,改行コード,コメントを削除。
				for (int i = 0; i < t_value.size(); ++i) {
					if (!this->IsSkipChar(t_value[i])) {
						if (t_value[i] == '/')i = this->GetNextNewLineSign(t_value, i);
						else value += t_value[i];
					}
				}
			}
			auto equal = value.find('=');
			if (equal == std::string::npos) {
				//exception(文法エラー)
				//変数名しか指定されていない(=がない)ことを示す。
				s3d::MessageBox::Show(L"文法エラーです。\n'='がありません。");
				return;
			}
			if (equal == value.size() - 1) {
				//exception(文法エラー)
				//変数の値がないことを示す。
				s3d::MessageBox::Show(L"文法エラーです。\n変数の値がありません。");
				return;
			}
			//=の右側にAnyDataListと関数の構造({})がない場合のみ=二つエラーを出す。
			//{でしか判定していないのは、}が無かった場合ここでエラーを出すより再帰的に呼び出したAnyDataListの{と}の数が一致していないエラーのほうがわかりやすいため
			//関数の場合はif文とか実装したいから(死ぬ)
			if (value.find('{', equal + 1) == std::string::npos&&value.find('=', equal + 1) != std::string::npos) {
				//exception(文法エラー)
				//=が二つ以上あることを示す。
				s3d::MessageBox::Show(L"文法エラーです。\n'='が二つ以上発見しました。");
				return;
			}
			valuedata.push_back({ std::string(value.begin(), value.begin() + equal), std::string(value.begin() + equal + 1, value.end()) });
			if (pos == std::string::npos)break;
		}
	}
	//解析が終わったらそれぞれをboost::anyに変換していく
}
std::string::size_type AnyDataList::find(const std::string& String, const char& SearchCode, std::string::size_type StartPos, std::vector<std::array<char, 2>> SkipChar) {
	for (;; ++StartPos) {
		if (StartPos == String.size()) {
			return std::string::npos;
		}
		//コメント(//)の処理。改行コードを見つけるまでStartPosを進める。改行コードが見つからなかったら、探している文字はないのでnposを返す。
		if (String[StartPos] == '/' && StartPos + 1 < String.size() && String[StartPos + 1] == '/') {
			StartPos = this->GetNextNewLineSign(String, StartPos);
			if (StartPos == std::string::npos)return std::string::npos;
		}
		//文字列("*")の処理。"を見つけたら次の"を見つけるまでStartPosを進める。
		else if (String[StartPos] == '\"') {
			StartPos = String.find('\"', StartPos + 1);
			if (StartPos == std::string::npos) {
				//exception(文法エラー)
				//"のペアが見つからないことを示す
				s3d::MessageBox::Show(L"文法エラーです。\n\"のペアを見つけることができませんでした");
				return std::string::npos;
			}
		}
		//呼び飛ばす文字を飛ばす
		else {
			bool c = false;
			for (auto& t_skippair : SkipChar) {
				if (String[StartPos] == t_skippair[0]) {
					StartPos = this->find(String, t_skippair[1], StartPos + 1, SkipChar);
					if (StartPos == std::string::npos) {
						//exception(文法エラー)
						//読み飛ばす文字のペアが見つからない事を示す
						s3d::MessageBox::Show(s3d::Widen(std::string("文法エラーです。\nペアを見つけることができませんでした")));
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
		//空白(半角のみ),タブ改行コード(\n,\r)はスキップする。
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
	//改行コードまで読み込むが、改行コードが2バイトだった場合はその分調整する。
	auto result = String.find('\n', StartPos);
	if (result == std::string::npos)return std::string::npos;
	if (result + 1 < String.size() && String[result + 1] == '\r')return result + 1;
	else return result;
}