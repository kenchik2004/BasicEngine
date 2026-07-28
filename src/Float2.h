//---------------------------------------------------------------------------
/**
 * @file   Float2.h
 * @brief  2次元の浮動小数点数ベクトル(float2)クラスの定義ヘッダーです。
 *         ゲーム開発における2次元の座標(X, Y)や移動方向、速度などを表現するのに使用します。
 */
//---------------------------------------------------------------------------
#pragma once

/**
 * @class float2
 * @brief 2次元浮動小数点ベクトルを扱うクラスです。
 *        このクラスを使用することで、2次元空間上の位置ベクトルや、速度ベクトルの計算を
 *        直感的かつ容易に行うことができます。
 */
class float2
{
public:
	float x; //!< X成分（横方向の位置や力）
	float y; //!< Y成分（縦方向の位置や力）

	/**
	 * @brief デフォルトコンストラクタ。
	 *        メンバ変数 x, y を 0.0f で安全に初期化します。
	 *        初期値が不定値（ゴミ値）になるのを防ぐための重要な初期設定です。
	 */
	float2();

	/**
	 * @brief 任意の値を指定して初期化するコンストラクタ。
	 * @param x 設定したい初期のX成分
	 * @param y 設定したい初期のY成分
	 */
	float2(float x, float y);

	/**
	 * @brief ベクトルの各成分を0にクリアします。
	 *        オブジェクトの位置や速度を原点、または静止状態にリセットする際に役立ちます。
	 */
	void clear();

	/**
	 * @brief 2つの浮動小数点数を直接指定してベクトルの成分を再設定します。
	 * @param x 新しいX成分
	 * @param y 新しいY成分
	 */
	void set(float x, float y);

	/**
	 * @brief 他の float2 オブジェクトの値をコピーしてベクトルの成分を設定します。
	 * @param v コピー元となる float2 オブジェクトへの参照
	 */
	void set(float2& v);

	/**
	 * @brief ベクトルの長さ（大きさ）を取得します。
	 *        三平方の定理（ピタゴラスの定理：sqrt(x^2 + y^2)）を応用して距離を求めます。
	 * @return ベクトルの長さ（float型）
	 */
	float GetLength();

	/**
	 * @brief ベクトルを正規化（長さを1に変換）します。
	 *        このメソッド自身が持つ x, y の成分が直接変更されます。
	 *        向きだけを表す単位ベクトル（方向ベクトル）に変換するために使用します。
	 */
	void normalize();

	/**
	 * @brief 正規化された新しいベクトルを取得します（自身の成分は変更しません）。
	 *        現在のベクトルの向きを保ったまま、長さが1である新しい float2 オブジェクトを返します。
	 * @return 正規化された新しい float2 ベクトル
	 */
	float2 normalized();

	/**
	 * @brief ベクトルの長さを指定された値に変更します。
	 *        現在のベクトルの向きを維持したまま、大きさを特定の強さ（速度など）に調整します。
	 * @param length 設定したい新しいベクトルの長さ
	 */
	void SetLength(float length);

	/**
	 * @brief 代入演算子（=）のオーバーロード。
	 *        別の float2 オブジェクトの成分を自身にコピーします。
	 * @param v コピー元となる float2 オブジェクトへの定数参照
	 * @return 自身（*this）への参照。連続した代入（a = b = c）を可能にするためです。
	 */
	float2& operator = (const float2& v);

	/**
	 * @brief 加算代入演算子（+=）のオーバーロード。
	 *        自身の成分に、別のベクトルの成分を加算します（x += v.x, y += v.y）。
	 * @param v 加算する float2 オブジェクトへの定数参照
	 * @return 加算後の自身（*this）への参照
	 */
	float2& operator += (const float2& v);

	/**
	 * @brief 減算代入演算子（-=）のオーバーロード。
	 *        自身の成分から、別のベクトルの成分を減算します（x -= v.x, y -= v.y）。
	 * @param v 減算する float2 オブジェクトへの定数参照
	 * @return 減算後の自身（*this）への参照
	 */
	float2& operator -= (const float2& v);

	/**
	 * @brief 乗算代入演算子（*=）のオーバーロード。
	 *        ベクトルの各成分に、指定された実数（スカラー値）を掛け合わせます。
	 * @param f 掛け合わせる倍率（スカラー値）
	 * @return 乗算後の自身（*this）への参照
	 */
	float2& operator *= (const float f);

	/**
	 * @brief 除算代入演算子（/=）のオーバーロード。
	 *        ベクトルの各成分を、指定された実数（スカラー値）で割ります。
	 * @param f 割る値（スカラー値）。※0を渡すとゼロ除算が発生するため注意が必要です。
	 * @return 除算後の自身（*this）への参照
	 */
	float2& operator /= (const float f);
};

/**
 * @brief 2つのベクトルの加算を行う二項演算子（+）のオーバーロード。
 *        (v1.x + v2.x, v1.y + v2.y) を計算した新しいベクトルを返します。
 * @param v1 左辺のベクトル
 * @param v2 右辺のベクトル
 * @return 加算結果の新しい float2 オブジェクト
 */
float2 operator + (const float2& v1, const float2& v2);

/**
 * @brief 2つのベクトルの減算を行う二項演算子（-）のオーバーロード。
 *        (v1.x - v2.x, v1.y - v2.y) を計算した新しいベクトルを返します。
 *        v2からv1への方向や距離を求める際によく使用されます。
 * @param v1 左辺のベクトル
 * @param v2 右辺のベクトル
 * @return 減算結果の新しい float2 オブジェクト
 */
float2 operator - (const float2& v1, const float2& v2);

/**
 * @brief ベクトルに実数（スカラー）を掛け合わせる二項演算子（*）のオーバーロード。
 *        (v.x * f, v.y * f) を計算した新しいベクトルを返します。
 * @param v 掛け算の対象となるベクトル
 * @param f 掛け合わせる倍率（スカラー値）
 * @return 乗算結果の新しい float2 オブジェクト
 */
float2 operator * (const float2& v, const float f);

/**
 * @brief ベクトルを実数（スカラー）で割る二項演算子（/）のオーバーロード。
 *        (v.x / f, v.y / f) を計算した新しいベクトルを返します。
 * @param v 割り算の対象となるベクトル
 * @param f 割る値（スカラー値）
 * @return 除算結果の新しい float2 オブジェクト
 */
float2 operator / (const float2& v, const float f);

/**
 * @brief 2つの座標点（位置ベクトル）間の直線距離を計算します。
 *        内部的には、pos1 から pos2 を引いた差分ベクトルの長さを求めています。
 * @param pos1 1つ目の位置座標への参照
 * @param pos2 2つ目の位置座標への参照
 * @return 2点間の直線距離（float型）
 */
float GetFloat2Distance(float2& pos1, float2& pos2);

/**
 * @brief 2つのベクトルの内積（ドット積）を求めます。
 *        計算式：v1.x * v2.x + v1.y * v2.y
 *        内積は、2つのベクトルのなす角度や、ある方向への投影、前後判定等に幅広く応用されます。
 * @param v1 1つ目のベクトルへの参照
 * @param v2 2つ目のベクトルへの参照
 * @return 内積の計算結果（float型）
 */
float GetFloat2Dot(float2& v1, float2& v2);

/**
 * @brief 2つの2次元ベクトルの外積（クロス積）のZ成分相当の値を求めます。
 *        計算式：v1.x * v2.y - v1.y * v2.x
 *        2次元の外積は、2つのベクトルの位置関係（右側にあるか左側にあるか）や
 *        三角形の面積、回転方向の判定などに非常に役立ちます。
 * @param v1 1つ目のベクトルへの参照
 * @param v2 2つ目のベクトルへの参照
 * @return 外積の計算結果（float型、正・負・ゼロで左右判定が可能）
 */
float GetFloat2Cross(float2& v1, float2& v2);
