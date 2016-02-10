/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Microsoft Corporation
 * 
 * -=- Robust Distributed System Nucleus (rDSN) -=- 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * Description:
 *     useful utilities in rDSN exposed via C API
 *
 * Revision history:
 *     Feb., 2016, @imzhenyu (Zhenyu Guo), first version
 *     xxxx-xx-xx, author, fix bug about xxx
 */

# pragma once

# include <dsn/c/api_common.h>

# ifdef __cplusplus
extern "C" {
# endif

/*!
\defgroup app-cli application command-line interface
\ingroup service-api-c
@{
*/

/*!
\brief built-in command-line interface 

rDSN allows apps/tools to register commands into its command line interface,
which can be further probed via local/remote console, and also http services.
*/
typedef struct dsn_cli_reply
{
    const char* message;  ///< zero-ended reply message
    uint64_t size;        ///< message_size
    void* context;        ///< context for free_handler
} dsn_cli_reply;

typedef void(*dsn_cli_handler)(
    void* context,              ///< context registered by dsn_cli_register
    int argc,                   ///< argument count
    const char** argv,          ///< arguments
    /*out*/dsn_cli_reply* reply ///< reply message
    );
typedef void(*dsn_cli_free_handler)(dsn_cli_reply reply);

extern DSN_API const char* dsn_cli_run(const char* command_line); // return command output
extern DSN_API void        dsn_cli_free(const char* command_output);

// return value: Handle (the handle of this registered command)
extern DSN_API dsn_handle_t dsn_cli_register(
                            const char* command,
                            const char* help_one_line,
                            const char* help_long,
                            void* context,
                            dsn_cli_handler cmd_handler,
                            dsn_cli_free_handler output_freer
                            );
// return value: Handle (the handle of this registered command)
// or NULL (We did no find a service_node, probably you call this function from a non-rdsn thread)
extern DSN_API dsn_handle_t dsn_cli_app_register(
                            const char* command,   // auto-augmented by rDSN as $app_full_name.$command
                            const char* help_one_line,
                            const char* help_long,
                            void* context,         // context to be forwareded to cmd_handler
                            dsn_cli_handler cmd_handler,
                            dsn_cli_free_handler output_freer
                            );

// remove a cli handler, parameter: return value of dsn_cli_register or dsn_cli_app_register
extern DSN_API void dsn_cli_deregister(dsn_handle_t cli_handle);
/*@}*/


/*!
\defgroup config configuration
\ingroup service-api-c
@{
*/

/*!
\brief configuration

Configuration provides an convenient way for developers to config their apps.
Note developers can change the underneath logging provider using rDSN's tool API so
that logs can be processed in a way that people are familiar with.
*/

extern DSN_API const char*           dsn_config_get_value_string(
                                        const char* section,       // [section]
                                        const char* key,           // key = value
                                        const char* default_value, // if [section] key is not present
                                        const char* dsptr          // what it is for, as help-info in config
                                        );
extern DSN_API bool                  dsn_config_get_value_bool(
                                        const char* section, 
                                        const char* key, 
                                        bool default_value, 
                                        const char* dsptr
                                        );
extern DSN_API uint64_t              dsn_config_get_value_uint64(
                                        const char* section, 
                                        const char* key, 
                                        uint64_t default_value, 
                                        const char* dsptr
                                        );
extern DSN_API double                dsn_config_get_value_double(
                                        const char* section, 
                                        const char* key, 
                                        double default_value, 
                                        const char* dsptr
                                        );
// return all key count (may greater than buffer_count)
extern DSN_API int                   dsn_config_get_all_keys(
                                        const char* section, 
                                        const char** buffers, 
                                        /*inout*/ int* buffer_count
                                        );
extern DSN_API void                  dsn_config_dump(const char* file);
/*@}*/

/*!
\defgroup logging logging facilities
\ingroup service-api-c
@{
*/

/*!
\brief logging facilities

Logging provides several macros for
*/
typedef enum dsn_log_level_t
{
    LOG_LEVEL_INFORMATION,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_COUNT,
    LOG_LEVEL_INVALID
} dsn_log_level_t;

// logs with level smaller than this start_level will not be logged
extern DSN_API dsn_log_level_t       dsn_log_start_level;
extern DSN_API dsn_log_level_t       dsn_log_get_start_level();
extern DSN_API void                  dsn_logv(
                                        const char *file, 
                                        const char *function, 
                                        const int line, 
                                        dsn_log_level_t log_level, 
                                        const char* title, 
                                        const char* fmt, 
                                        va_list args
                                        );
extern DSN_API void                  dsn_logf(
                                        const char *file, 
                                        const char *function, 
                                        const int line, 
                                        dsn_log_level_t log_level, 
                                        const char* title, 
                                        const char* fmt, 
                                        ...
                                        );
extern DSN_API void                  dsn_log(
                                        const char *file, 
                                        const char *function, 
                                        const int line, 
                                        dsn_log_level_t log_level,
                                        const char* title
                                        );
extern DSN_API void                  dsn_coredump();


#define dlog(level, title, ...) do {if (level >= dsn_log_start_level) \
        dsn_logf(__FILE__, __FUNCTION__, __LINE__, level, title, __VA_ARGS__); } while(false)
#define dinfo(...)  dlog(LOG_LEVEL_INFORMATION, __TITLE__, __VA_ARGS__)
#define ddebug(...) dlog(LOG_LEVEL_DEBUG, __TITLE__, __VA_ARGS__)
#define dwarn(...)  dlog(LOG_LEVEL_WARNING, __TITLE__, __VA_ARGS__)
#define derror(...) dlog(LOG_LEVEL_ERROR, __TITLE__, __VA_ARGS__)
#define dfatal(...) dlog(LOG_LEVEL_FATAL, __TITLE__, __VA_ARGS__)
#define dassert(x, ...) do { if (!(x)) {                    \
            dlog(LOG_LEVEL_FATAL, __FILE__, "assertion expression: "#x); \
            dlog(LOG_LEVEL_FATAL, __FILE__, __VA_ARGS__);  \
            dsn_coredump();       \
                } } while (false)

#ifndef NDEBUG
#define dbg_dassert dassert
#else
#define dbg_dassert(x, ...) 
#endif

/*@}*/



//------------------------------------------------------------------------------
//
// checksum using crc
//
//------------------------------------------------------------------------------

extern DSN_API uint32_t              dsn_crc32_compute(const void* ptr, size_t size, uint32_t init_crc);

//
// Given
//      x_final = dsn_crc32_compute (x_ptr, x_size, x_init);
// and
//      y_final = dsn_crc32_compute (y_ptr, y_size, y_init);
// compute CRC of concatenation of A and B
//      x##y_crc = dsn_crc32_compute (x##y, x_size + y_size, xy_init);
// without touching A and B
//
extern DSN_API uint32_t              dsn_crc32_concatenate(
                                        uint32_t xy_init, 
                                        uint32_t x_init,
                                        uint32_t x_final, 
                                        size_t   x_size, 
                                        uint32_t y_init, 
                                        uint32_t y_final, 
                                        size_t   y_size
                                        );

extern DSN_API uint64_t               dsn_crc64_compute(const void* ptr, size_t size, uint64_t init_crc);

//
// Given
//      x_final = dsn_crc64_compute (x_ptr, x_size, x_init);
// and
//      y_final = dsn_crc64_compute (y_ptr, y_size, y_init);
// compute CRC of concatenation of A and B
//      x##y_crc = dsn_crc64_compute (x##y, x_size + y_size, xy_init);
// without touching A and B
//

extern DSN_API uint64_t              dsn_crc64_concatenate(
                                        uint32_t xy_init,
                                        uint64_t x_init,
                                        uint64_t x_final,
                                        size_t x_size,
                                        uint64_t y_init,
                                        uint64_t y_final,
                                        size_t y_size);


//------------------------------------------------------------------------------
//
// perf counters
//
//------------------------------------------------------------------------------

typedef enum dsn_perf_counter_type_t
{
    COUNTER_TYPE_NUMBER,
    COUNTER_TYPE_RATE,
    COUNTER_TYPE_NUMBER_PERCENTILES,
    COUNTER_TYPE_INVALID,
    COUNTER_TYPE_COUNT
} dsn_perf_counter_type_t;

typedef enum dsn_perf_counter_percentile_type_t
{
    COUNTER_PERCENTILE_50,
    COUNTER_PERCENTILE_90,
    COUNTER_PERCENTILE_95,
    COUNTER_PERCENTILE_99,
    COUNTER_PERCENTILE_999,

    COUNTER_PERCENTILE_COUNT,
    COUNTER_PERCENTILE_INVALID
} dsn_perf_counter_percentile_type_t;

extern DSN_API dsn_handle_t dsn_perf_counter_create(const char* section, const char* name, dsn_perf_counter_type_t type, const char* description);
extern DSN_API void dsn_perf_counter_remove(dsn_handle_t handle);
extern DSN_API void dsn_perf_counter_increment(dsn_handle_t handle);
extern DSN_API void dsn_perf_counter_decrement(dsn_handle_t handle);
extern DSN_API void dsn_perf_counter_add(dsn_handle_t handle, uint64_t val);
extern DSN_API void dsn_perf_counter_set(dsn_handle_t handle, uint64_t val);
extern DSN_API double dsn_perf_counter_get_value(dsn_handle_t handle);
extern DSN_API uint64_t dsn_perf_counter_get_integer_value(dsn_handle_t handle);
extern DSN_API double dsn_perf_counter_get_percentile(dsn_handle_t handle, dsn_perf_counter_percentile_type_t type);

# ifdef __cplusplus
}
# endif