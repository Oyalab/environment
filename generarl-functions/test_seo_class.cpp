#include <gtest/gtest.h>
#include <stdexcept>
#include "seo_class.hpp"

// GoogleTestのSetUpをオーバーライドしてパラメータのセットアップ
class SEOTest : public ::testing::Test
{
protected:
    unique_ptr<vector<vector<vector<shared_ptr<SEO>>>>> seoGrid;

    void SetUp() override
    {
        int x = 2, y = 2, z = 2;

        // 3次元ベクトル (サイズ: 2x2x2) を動的に生成
        seoGrid = make_unique<vector<vector<vector<shared_ptr<SEO>>>>>(
            z, vector<vector<shared_ptr<SEO>>>(y, vector<shared_ptr<SEO>>(x)));

        // 各要素を生成して格納
        for (int i = 0; i < z; ++i)
        {
            for (int j = 0; j < y; ++j)
            {
                for (int k = 0; k < x; ++k)
                {
                    (*seoGrid).at(i).at(j).at(k) = make_shared<SEO>(1.0, 0.001, 18.0, 2.0, 0.007, 3);
                }
            }
        }

        // [0][0][0]の振動子に[1][0][0],[0][1][0],[0][0][1]を接続
        vector<shared_ptr<SEO>> connections = {
            (*seoGrid).at(1).at(0).at(0),
            (*seoGrid).at(0).at(1).at(0),
            (*seoGrid).at(0).at(0).at(1)};
        (*seoGrid).at(0).at(0).at(0)->setConnections(connections);
    }
};

// テストケース: コンストラクタの動作確認
TEST_F(SEOTest, Constructor)
{
    // パラメータの検証
    EXPECT_EQ((*seoGrid).at(0).at(0).at(0)->getR(), 1.0);
    EXPECT_EQ((*seoGrid).at(0).at(0).at(1)->getRj(), 0.001);
    EXPECT_EQ((*seoGrid).at(0).at(1).at(0)->getCj(), 18.0);
    EXPECT_EQ((*seoGrid).at(1).at(0).at(0)->getC(), 2);
    EXPECT_EQ((*seoGrid).at(0).at(1).at(1)->getVd(), 0.007);
    EXPECT_EQ((*seoGrid).at(1).at(1).at(1)->getlegs(), 3);
}

// テストケース: 接続されてる振動子のポインタの追加確認
TEST_F(SEOTest, AddConnections)
{
    // 接続されたポインタが正しいか確認
    auto connections = (*seoGrid).at(0).at(0).at(0)->getConnection();
    EXPECT_EQ(connections.size(), 3);
    // 接続0: [1][0][0]
    EXPECT_EQ(connections[0], (*seoGrid).at(1).at(0).at(0));
    // 接続1: [0][1][0]
    EXPECT_EQ(connections[1], (*seoGrid).at(0).at(1).at(0));
    // 接続2: [0][0][1]
    EXPECT_EQ(connections[2], (*seoGrid).at(0).at(0).at(1));

    // 新しい接続のベクトルを作成
    vector<shared_ptr<SEO>> tooManyConnections = {
        (*seoGrid).at(1).at(0).at(0),
        (*seoGrid).at(0).at(1).at(0),
        (*seoGrid).at(0).at(0).at(1),
        (*seoGrid).at(1).at(1).at(1)}; // 4つ目の接続で超過

    // 例外がスローされるか確認
    EXPECT_THROW((*seoGrid).at(0).at(0).at(0)->setConnections(tooManyConnections), invalid_argument);

    // 自分自身への接続
    vector<shared_ptr<SEO>> selfConnections = {
        (*seoGrid).at(0).at(0).at(0),
        (*seoGrid).at(1).at(0).at(0),
        (*seoGrid).at(0).at(1).at(0)};
    // 例外がスローされるか確認
    EXPECT_THROW((*seoGrid).at(0).at(0).at(0)->setConnections(selfConnections), invalid_argument);
}

// テストケース: 接続されてる振動子のノード電圧を参照する動作確認

// テストケース: パラメータ計算の動作確認

// テストケース: calculateTunnelWt の動作確認
TEST_F(SEOTest, CalculateTunnelWt)
{
    // 振動子のセットアップ
    SEO seo(1, 0.001, 18, 2, 0.007, 1);

    // エネルギー変化量をセット
    seo.setdE("up", 1);    // 上方向のエネルギー変化量
    seo.setdE("down", -1); // 下方向のエネルギー変化量

    // トンネル待ち時間を計算
    seo.calculateTunnelWt();
    // 結果を検証
    EXPECT_NE(seo.getWT("up"), 0);
    EXPECT_EQ(seo.getWT("down"), 0);

    // エネルギー変化量をセット
    seo.setdE("up", -1);  // 上方向のエネルギー変化量
    seo.setdE("down", 1); // 下方向のエネルギー変化量

    // トンネル待ち時間を計算
    seo.calculateTunnelWt();
    // 結果を検証
    EXPECT_EQ(seo.getWT("up"), 0);
    EXPECT_NE(seo.getWT("down"), 0);
}