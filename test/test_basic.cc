#include <gtest/gtest.h>
#include "src/txn.h"

class TxnTest : public testing::Test {
public:
    
    void SetUp() override {}
    void TearDown() override {}

private:
};

TEST_F(TxnTest, basic) {
    KeyValuePair p1;
    p1.key = "1";
    p1.value.value = "1 -> version1";
    KeyValuePair p2;
    p2.key = "2";
    p2.value.value = "2 -> version1";
    KeyValuePair p3;
    p3.key = "3";
    p3.value.value = "3 -> version1";
    KeyValuePairVec vec; 
    vec.push_back(p1);
    vec.push_back(p2);
    vec.push_back(p3);

    Txn t1;
    t1.init(vec);
    EXPECT_TRUE(t1.prewrite());
    EXPECT_TRUE(t1.commit());

    Peek peek1;
    std::string value;
    EXPECT_TRUE(peek1.read(p1.key, value));
    EXPECT_EQ(value, p1.value.value);

    Peek peek2;
    value.clear();
    EXPECT_TRUE(peek2.read(p2.key, value));
    EXPECT_EQ(value, p2.value.value);

    Peek peek3;
    value.clear();
    EXPECT_TRUE(peek3.read(p3.key, value));
    EXPECT_EQ(value, p3.value.value);

    // let's pad some version
    p1.value.value = "1 -> version2";
    Txn t2;
    vec.clear();
    vec.push_back(p1);
    t2.init(vec);
    EXPECT_TRUE(t2.prewrite());
    EXPECT_TRUE(t2.commit());

    Peek peek4;
    value.clear();
    EXPECT_TRUE(peek4.read(p1.key, value));
    EXPECT_EQ(value, p1.value.value);

    Peek peek5;
    value.clear();
    EXPECT_TRUE(peek5.read(p2.key, value));
    EXPECT_EQ(value, p2.value.value);
}

TEST_F(TxnTest, conflict) {
    // TODO:
}

TEST_F(TxnTest, abort) {
    // TODO:
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
