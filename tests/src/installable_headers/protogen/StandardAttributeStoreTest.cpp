#include <algorithm>

#include <gtest/gtest.h>

#include <protogen/IAttributeStore.hpp>
#include <protogen/StandardAttributeStore.hpp>

class StandardAttributeStoreTest : public testing::Test {
protected:
    StandardAttributeStoreTest() {
        using namespace protogen::attributes;
        store.setAttribute("key", "value");
        store.setAttribute("key2", "value2");
        store.setAttribute("key3", "value3");
        store.setAttribute("key4", "value4");
        store.setAttribute("empty", "");
    }

    protogen::StandardAttributeStore store;
};

TEST_F(StandardAttributeStoreTest, SetAttribute) {
    using namespace protogen;
    using namespace protogen::attributes;

    // default access
    EXPECT_EQ(store.setAttribute("key", "value"), IAttributeStore::SetAttributeResult::Updated);
    EXPECT_EQ(store.getAttribute("key").value(), "value");

    // create new attribute
    EXPECT_EQ(store.setAttribute("key12345", "value"), IAttributeStore::SetAttributeResult::Created);
    EXPECT_EQ(store.getAttribute("key12345").value(), "value");
}

TEST_F(StandardAttributeStoreTest, GetAttribute) {
    EXPECT_EQ(store.getAttribute("key").value(), "value");
    EXPECT_EQ(store.getAttribute("key2").value(), "value2");
    EXPECT_EQ(store.getAttribute("non-key"), std::optional<std::string>());
}

TEST_F(StandardAttributeStoreTest, ListAttributes) {
    std::vector<std::string> expected = {"key", "key2", "key3", "key4", "empty"};
    std::vector<std::string> actual = store.listAttributes();
    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());
    EXPECT_EQ(actual, expected);
}

TEST_F(StandardAttributeStoreTest, RemoveAttribute) {
    using namespace protogen;
    using namespace protogen::attributes;

    // remove existing attribute
    EXPECT_EQ(store.removeAttribute("key"), IAttributeStore::RemoveAttributeResult::Removed);
    EXPECT_EQ(store.getAttribute("key"), std::optional<std::string>());

    // remove non-existing attribute
    EXPECT_EQ(store.removeAttribute("non-key"), IAttributeStore::RemoveAttributeResult::DoesNotExist);
    EXPECT_EQ(store.getAttribute("non-key"), std::optional<std::string>());
}

TEST_F(StandardAttributeStoreTest, HasAttribute) {
    // attribute exists
    EXPECT_TRUE(store.hasAttribute("key"));

    // does not exist
    EXPECT_FALSE(store.hasAttribute("non-key"));
}