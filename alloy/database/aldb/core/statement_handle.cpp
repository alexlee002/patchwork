//
//  statement_handle.cpp
//  alloy
//
//  Created by Alex Lee on 23/07/2017.
//  Copyright © 2017 Alex Lee. All rights reserved.
//

#include "statement_handle.hpp"
#include "sql_value.hpp"
#include <sqlite3.h>
#include "Error.hpp"
#include "defines.hpp"
#include <assert.h>
#include <pthread.h>

namespace aldb {
StatementHandle::StatementHandle(const Handle &handle, void *stmt)
    : aldb::Catchable()
    , _hadler(handle)
    , _stmt(stmt) {}
    
StatementHandle::~StatementHandle() { finalize(); }

void StatementHandle::finalize() {
    if (!_stmt) {
        return;
    }
    
    sqlite3 *h = sqlite3_db_handle((sqlite3_stmt *) _stmt);
    int rc = sqlite3_finalize((sqlite3_stmt *) _stmt);
    _stmt = nullptr;
    if (rc == SQLITE_OK) {
        Catchable::reset_error();
        return;
    }
    Catchable::set_sqlite_error(h);
    Catchable::log_error();
}

bool StatementHandle::step() {
    if (!_stmt) {
        return false;
    }
    
//    __uint64_t threadid;
//    pthread_threadid_np(NULL, &threadid);
//    if (this->threadid != threadid) {
//        printf("❗️current thread:%lld, handle[%p] thread:%lld\n", threadid, this, this->threadid);
//        assert(false);
//    }
    

    int rc = sqlite3_step((sqlite3_stmt *) _stmt);
    if (rc == SQLITE_ROW || rc == SQLITE_OK || rc == SQLITE_DONE) {
        Catchable::reset_error();
        return rc == SQLITE_ROW;
    }
    
    Catchable::set_sqlite_error(sqlite3_db_handle((sqlite3_stmt *) _stmt));
    Catchable::log_error();
    return false;
}

bool StatementHandle::reset_bindings() {
    if (!_stmt) {
        //TODO: assert?
        return false;
    }
    int rc = sqlite3_reset((sqlite3_stmt *) _stmt);
    if (rc == SQLITE_OK) {
        Catchable::reset_error();
        return true;
    } else {
        Catchable::set_sqlite_error(sqlite3_db_handle((sqlite3_stmt *) _stmt));
        Catchable::log_error();
        return false;
    }
}

bool StatementHandle::bind_value(const SQLValue &value, const int index) {
    int rc = SQLITE_OK;
    switch (value.val_type) {
        case aldb::ColumnType::INT32_T:
            rc = sqlite3_bind_int((sqlite3_stmt *) _stmt, index, value.i32_val);
            break;
        case aldb::ColumnType::INT64_T:
            rc = sqlite3_bind_int64((sqlite3_stmt *) _stmt, index, value.i64_val);
            break;
        case aldb::ColumnType::DOUBLE_T:
            rc = sqlite3_bind_double((sqlite3_stmt *) _stmt, index, value.d_val);
            break;
        case aldb::ColumnType::TEXT_T:
            rc = sqlite3_bind_text((sqlite3_stmt *) _stmt, index, value.s_val.c_str(), -1, SQLITE_STATIC);
            break;
        case aldb::ColumnType::BLOB_T:
            rc = sqlite3_bind_blob((sqlite3_stmt *) _stmt, index, value.s_val.c_str(), -1, SQLITE_STATIC);
            break;
        case aldb::ColumnType::NULL_T:
            rc = sqlite3_bind_null((sqlite3_stmt *) _stmt, index);
            break;

        default:
            rc = sqlite3_bind_blob((sqlite3_stmt *) _stmt, index, std::string(value).c_str(), -1, SQLITE_STATIC);
            break;
    }
    if (rc != SQLITE_OK) {
        Catchable::set_sqlite_error(sqlite3_db_handle((sqlite3_stmt *) _stmt));
        Catchable::log_error();
        return false;
    } else {
        Catchable::reset_error();
        return true;
    }
}

const int32_t StatementHandle::get_int32_value(int index) {
    return (int32_t) sqlite3_column_int((sqlite3_stmt *) _stmt, index);
}

const int64_t StatementHandle::get_int64_value(int index) {
    return (int64_t) sqlite3_column_int64((sqlite3_stmt *) _stmt, index);
}

const double StatementHandle::get_double_value(int index) {
    return (double) sqlite3_column_double((sqlite3_stmt *) _stmt, index);
}

const char *StatementHandle::get_text_value(int index) {
    return (const char *) sqlite3_column_text((sqlite3_stmt *) _stmt, index);
}
const void *StatementHandle::get_blob_value(int index, size_t &size) {
    size = sqlite3_column_bytes((sqlite3_stmt *) _stmt, index);
    return (const void *) sqlite3_column_blob((sqlite3_stmt *) _stmt, index);
}

int StatementHandle::column_count() const { return sqlite3_column_count((sqlite3_stmt *) _stmt); }

const char *StatementHandle::column_name(int idx) const { return sqlite3_column_name((sqlite3_stmt *) _stmt, idx); }

ColumnType StatementHandle::column_type(int idx) const {
    int type = sqlite3_column_type((sqlite3_stmt *) _stmt, idx);
    switch (type) {
        case SQLITE_INTEGER:
            return ColumnType::INT64_T;
        case SQLITE_FLOAT:
            return ColumnType::DOUBLE_T;
        case SQLITE_BLOB:
            return ColumnType::BLOB_T;
        case SQLITE_NULL:
            return ColumnType::NULL_T;
        case SQLITE_TEXT:
            return ColumnType::TEXT_T;

        default:
            return ColumnType::BLOB_T;
    }
}
}
