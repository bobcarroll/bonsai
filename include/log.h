/**
 * Bonsai - open source group collaboration and application lifecycle management
 * Copyright (c) 2011 Bob Carroll
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#define GCS_LOG_FATAL       0   /* encountered a serious, unrecoverable error */
#define GCS_LOG_CRITICAL    1   /* encountered a serious, recoverable error */
#define GCS_LOG_ERROR       2   /* encountered a minor error state or condition */
#define GCS_LOG_WARN        3   /* encountered a potentially undesirable state or condition */ 
#define GCS_LOG_NOTICE      4   /* important (non-error) messages */
#define GCS_LOG_INFO        5   /* status information for the curious (or obsessive) type */
#define GCS_LOG_DEBUG       6   /* verbose debug information for developers */
#define GCS_LOG_TRACE       7   /* extra verbose debug information for developers */

void gcs_log_close();
int gcs_log_level();
int gcs_log_open(const char *, int, int);
void gcs_log_write(int, const char *, int, const char *format, ...);

#define gcslog_fatal(msg, args...) \
    gcs_log_write(GCS_LOG_FATAL, __FUNCTION__, __LINE__, msg, ##args)
#define gcslog_critical(msg, args...) \
    gcs_log_write(GCS_LOG_CRITICAL, __FUNCTION__, __LINE__, msg, ##args)
#define gcslog_error(msg, args...) \
    gcs_log_write(GCS_LOG_ERROR, __FUNCTION__, __LINE__, msg, ##args)
#define gcslog_warn(msg, args...) \
    gcs_log_write(GCS_LOG_WARN, __FUNCTION__, __LINE__, msg, ##args)
#define gcslog_notice(msg, args...) \
    gcs_log_write(GCS_LOG_NOTICE, __FUNCTION__, __LINE__, msg, ##args)
#define gcslog_info(msg, args...) \
    gcs_log_write(GCS_LOG_INFO, __FUNCTION__, __LINE__, msg, ##args)
#define gcslog_debug(msg, args...) \
    gcs_log_write(GCS_LOG_DEBUG, __FUNCTION__, __LINE__, msg, ##args)
#define gcslog_trace(msg, args...) \
    gcs_log_write(GCS_LOG_TRACE, __FUNCTION__, __LINE__, msg, ##args)

