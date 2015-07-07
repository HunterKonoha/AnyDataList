#pragma once
#include <string>
#include <vector>
#include <array>

class AnyDataList {
	private:

	private:
		void PreInit();
		/// <summary>
		/// 指定された位置から最初に見つかった改行コード(\n,\n\r)の終わりの位置を返します。
		/// </summary>
		/// <param name="String">検索する文字列</param>
		/// <param name="StartPos">初期位置</param>
		/// <returns>改行コード(\n,\n\r)の終わりの位置</returns>
		std::string::size_type GetNextNewLineSign(const std::string& String, std::string::size_type StartPos = 0);

	public:
		AnyDataList(std::string e);
		~AnyDataList() {};
		/// <summary>
		/// コメントや""を考慮しながら文字列を指定した位置から検索して、最初に指定した文字があった場所を返す。
		/// </summary>
		/// <param name="String">検索する文字列</param>
		/// <param name="SearchCode">指定する文字</param>
		/// <param name="StartPos">最初の位置</param>
		/// <param name="SkipChar">追加の読み飛ばす文字のペア</param>
		/// <returns>最初に文字列があった場所</returns>
		std::string::size_type find(const std::string& String, const char& SearchCode, std::string::size_type StartPos = 0, std::vector<std::array<char, 2>> SkipChar = {});
		/// <summary>
		/// 文字がスキップ(無視)される文字かどうか判定します。
		/// </summary>
		/// <param name="Char">判定する文字</param>
		/// <returns>スキップ(無視)するかどうか</returns>
		bool IsSkipChar(const char& Char);
};