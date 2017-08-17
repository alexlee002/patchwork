//
//  ALDBConnectionDelegate.h
//  alloy
//
//  Created by Alex Lee on 27/07/2017.
//  Copyright © 2017 Alex Lee. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "aldb.h"

@class ALDatabase;
@protocol ALDBConnectionDelegate <NSObject>

@required
+ (BOOL)canOpenDatabaseWithPath:(nonnull in NSString *)path;

@optional
- (void)databaseDidOpen:(const aldb::RecyclableHandle)handle;
- (void)databaseDidReady:(const aldb::RecyclableHandle)handle;

- (void)willCloseDatabase:(ALDatabase *_Nonnull)database;
- (void)didCloseDatabase:(ALDatabase *_Nonnull)database;

@end
