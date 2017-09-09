//
//  ALSQLClause.h
//  alloy
//
//  Created by Alex Lee on 15/08/2017.
//  Copyright © 2017 Alex Lee. All rights reserved.
//

#import <string>
#import <list>
#import <Foundation/Foundation.h>
#import "sql_value.hpp"

NS_ASSUME_NONNULL_BEGIN

class ALSQLValue;
class ALSQLClause {
public:
    ALSQLClause();
    ALSQLClause(std::nullptr_t);
    ALSQLClause(const char *_Nonnull sql);
    ALSQLClause(const std::string &sql);
    ALSQLClause(const std::string &sql, const std::list<const ALSQLValue> &args);
    
    ALSQLClause(NSString *_Nonnull sql);
    ALSQLClause(NSString *_Nonnull sql, NSArray<id> *_Nonnull args);
    
    ALSQLClause(const ALSQLClause &clause);
    
    ALSQLClause &rebind_values(const std::list<const ALSQLValue> &args);
    
    ALSQLClause &append(const std::string &sql, const std::list<const ALSQLValue> &args);
    ALSQLClause &append(NSString *_Nonnull sql);
    ALSQLClause &append(const ALSQLClause &clause);
    ALSQLClause &append(const char * sql);
    
//    ALSQLClause &operator+=(const ALSQLClause &clause);
//    ALSQLClause operator+(const ALSQLClause &clause) const;
    
    ALSQLClause &operator=(const ALSQLClause &other);
    
    const std::string &sql_str() const;
    const std::list<const aldb::SQLValue> sql_args() const;
    
    NSString *_Nonnull sqlString() const;
    const std::list<const ALSQLValue> &sqlArgs() const;
    
    bool is_empty() const;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
    template <typename T, typename U>
    static typename std::enable_if<std::is_base_of<ALSQLClause, T>::value &&
                                   std::is_base_of<ALSQLClause, U>::value, T>::type
    combine(const std::list<const U> &clauses, const char *_Nullable delimiter) {
        T clause;
        bool flag = false;
        for (const U t : clauses) {
            if (flag) {
                clause._sql.append(delimiter);
            } else {
                flag = true;
            }
            clause.append(t);
        }
        return clause;
    }
#pragma clang diagnostic pop

  protected:
    std::string _sql;
    std::list<const ALSQLValue> _args;
    
};

NS_ASSUME_NONNULL_END
