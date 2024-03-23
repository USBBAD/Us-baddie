//
// debug.h
//
// Created on: March 23, 2024
//     Author: Dmitry Murashov (dmtr DOT murashov AT GMAIL)
//

#ifndef SRC_COMMON_UTILITY_DEBUG_H_
#define SRC_COMMON_UTILITY_DEBUG_H_

typedef void(*UsDebugCallable)(const void *);

/// \brief Each module is associated w/ a token
/// \returns token `>=0`, if successful
/// \pre No checks for contexts should be assumed, usDebugRegisterToken should be called once for each token
int usDebugRegisterToken(const char *aContext);

/// \brief Puts task into a queue
/// \returns >0, if successful
/// \post Will produce an error message, when failed
/// \pre `usDebugRegisterToken` must be called before to get `aToken`
int usDebugAddTask(int aToken, UsDebugCallable aCallable, const void *aArg);

/// \brief Pushes message w/o the need for formatting
/// \details CR, NL symbols will be added automatically
int usDebugPushMessage(int aToken, const char *aMessage);

void usDebugIterDebugLoop();

#endif  // SRC_COMMON_UTILITY_DEBUG_H_

