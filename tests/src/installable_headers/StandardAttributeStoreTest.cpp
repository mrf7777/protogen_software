#include <algorithm>

#include <gtest/gtest.h>

#include <protogen/IAttributeStore.hpp>
#include <protogen/StandardAttributeStore.hpp>

class StandardAttributeStoreTest : public testing::Test {
protected:
    StandardAttributeStoreTest() {
        using namespace protogen::attributes;
        store.adminSetAttribute("key", "value", IAttributeStore::Access::Read);
        store.adminSetAttribute("key2", "value2", IAttributeStore::Access::ReadWrite);
        store.adminSetAttribute("key3", "value3", IAttributeStore::Access::Write);
        store.adminSetAttribute("key4", "value4", IAttributeStore::Access::ReadWrite);
    }

    protogen::StandardAttributeStore store;
};

TEST_F(StandardAttributeStoreTest, SetAttribute) {
    using namespace protogen;
    using namespace protogen::attributes;

    // default access
    auto result = store.setAttribute("key12345", "value");
    ASSERT_EQ(result, IAttributeStore::SetAttributeResult::Created);
    const auto access = store.getAttributeAccess("key12345");
    const auto default_access = store.getDefaultAccess();
    EXPECT_EQ(access, default_access);

    // deny write to read-only attribute
    result = store.setAttribute("key", "new_value");
    EXPECT_EQ(result, IAttributeStore::SetAttributeResult::UnsetBecauseNotWritable);

    // can write on ReadWrite access
    result = store.setAttribute("key2", "new_value");
    EXPECT_EQ(result, IAttributeStore::SetAttributeResult::Updated);
    EXPECT_EQ(store.getAttribute("key2").value(), "new_value");

    // can write on Write access
    result = store.setAttribute("key3", "new_value");
    EXPECT_EQ(result, IAttributeStore::SetAttributeResult::Updated);
    EXPECT_EQ(store.adminGetAttribute("key3").value(), "new_value");
}

TEST_F(StandardAttributeStoreTest, GetAttribute) {
    EXPECT_EQ(store.getAttribute("key").value(), "value");
    EXPECT_EQ(store.getAttribute("key2").value(), "value2");
    EXPECT_EQ(store.getAttribute("key3"), std::optional<std::string>());
}

TEST_F(StandardAttributeStoreTest, ListAttributes) {
    std::vector<std::string> expected = {"key", "key2", "key3", "key4"};
    std::vector<std::string> actual = store.listAttributes();
    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());
    EXPECT_EQ(actual, expected);
}

TEST_F(StandardAttributeStoreTest, GetAttributeAccess) {
    using namespace protogen;
    using namespace protogen::attributes;

    // access for existing attributes
    auto access = store.getAttributeAccess("key");
    EXPECT_EQ(access.value(), IAttributeStore::Access::Read);
    access = store.getAttributeAccess("key2");
    EXPECT_EQ(access.value(), IAttributeStore::Access::ReadWrite);
    access = store.getAttributeAccess("key3");
    EXPECT_EQ(access.value(), IAttributeStore::Access::Write);

    // non-existent attribute
    access = store.getAttributeAccess("key12345");
    EXPECT_EQ(access, std::optional<IAttributeStore::Access>());
}

TEST_F(StandardAttributeStoreTest, RemoveAttribute) {
    using namespace protogen;
    using namespace protogen::attributes;

    // cant remove read-only
    auto result = store.removeAttribute("key");
    EXPECT_EQ(result, IAttributeStore::RemoveAttributeResult::KeptBecauseNotWritable);

    // cant remove what does not exist
    result = store.removeAttribute("key12345");
    EXPECT_EQ(result, IAttributeStore::RemoveAttributeResult::DoesNotExist);

    // remove Write and ReadWrite by default
    result = store.removeAttribute("key2");
    EXPECT_EQ(result, IAttributeStore::RemoveAttributeResult::Removed);
    result = store.removeAttribute("key3");
    EXPECT_EQ(result, IAttributeStore::RemoveAttributeResult::Removed);

    // cant remove pinned attribute
    store.pin("key4");
    result = store.removeAttribute("key4");
    EXPECT_EQ(result, IAttributeStore::RemoveAttributeResult::KeptBecauseImplementation);

    // remove unpinned attribute
    store.unpin("key4");
    result = store.removeAttribute("key4");
    EXPECT_EQ(result, IAttributeStore::RemoveAttributeResult::Removed);
}

TEST_F(StandardAttributeStoreTest, HasAttribute) {
    // attribute exists
    EXPECT_TRUE(store.hasAttribute("key"));

    // does not exist
    EXPECT_FALSE(store.hasAttribute("key12345"));
}