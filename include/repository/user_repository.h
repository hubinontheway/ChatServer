#pragma once

#include <memory>
#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include "user.hxx"
#include "user-odb.hxx"

using namespace std;

class UserRepository {
public:
    UserRepository(shared_ptr<odb::database> db) : db_(db) {
    }

    // 增加用户
    void createUser(const string &email, const string &name, const string &password) {
        User user = User(email, name, password);
        odb::transaction t(db_->begin());
        db_->persist(user);
        t.commit();
    }

    // 查找用户 by id
    shared_ptr<User> getUserById(unsigned long long id) {
        odb::transaction t(db_->begin());
        return shared_ptr<User>(db_->load<User>(id));
    }

    // 查找用户 by email（使用 query）
    shared_ptr<User> getUserByEmail(const string &email) {
        odb::transaction t(db_->begin());

        typedef odb::query<User> query;
        typedef odb::result<User> result;

        result r(db_->query<User>(query::email == email));
        if (!r.empty())
            return shared_ptr<User>(r.begin().load());

        return nullptr;
    }

    // 修改用户（必须是从数据库中获取并修改）
    void updateUser(shared_ptr<User> user) {
        odb::transaction t(db_->begin());
        db_->update(user.get());
        t.commit();
    }

    // 删除用户
    void deleteUser(unsigned long long id) {
        odb::transaction t(db_->begin());
        db_->erase<User>(id);
        t.commit();
    }

private:
    shared_ptr<odb::database> db_;
};
