#pragma once
#include <stdint.h>

/* Modified file.hpp from hollyhock sdk */

/**
 * @name Errors
 * Errors returned by file system functions. All negative numbers.
 * @{
 */
#define CAS_ENOMEM                  -1
#define CAS_EINVAL                  -2
#define CAS_EDEVFAIL                -3
#define CAS_EMOUNTED                -4
#define CAS_EACCES                  -5
#define CAS_EBADFSID                -6
#define CAS_ENOVOLUME               -7
#define CAS_ENOPATH                 -8
#define CAS_EEXIST                  -9
#define CAS_ENAMETOOLONG            -10
#define CAS_EOUTOFBOUND             -11
#define CAS_EUNFORMAT               -12
#define CAS_ENOSPC                  -13
#define CAS_ENOENT                  -14
#define CAS_EISDIRECTORY            -15
#define CAS_ESHARE                  -16
#define CAS_EMFILE                  -17
#define CAS_EBADF                   -18
#define CAS_EEOF                    -19
#define CAS_ENOTEMPTY               -20
#define CAS_ECLUSTERSIZEMISMATCH    -40
#define CAS_ESYSTEM                 -99
/// @}

/**
 * @defgroup open_flags_values open flags Values
 * Values passed as the @c flags parameter to @ref open. Can be bitwise OR'd to
 * combine effects.
 * @{
 */
/// Open the file as readable.
#define CAS_O_READ 1 << 0
/// Open the file as writable.
#define CAS_O_WRITE 1 << 1
/// Create the file, if it does not already exist.
#define CAS_O_CREATE 1 << 2
/// Opens the file with the file offset set to the end of the file.
#define CAS_O_APPEND 1 << 4
/// @}

/**
 * Retrieves the year from a @c struct @ref stat date field.
 * 
 * @param date The date field from a @c struct @ref stat.
 * @return The year encoded in the date field.
 */
uint16_t constexpr statDateYear(uint16_t date) {
	return ((date >> 9) & 0b1111111) + 1980;
}

/**
 * Retrieves the month from a @c struct @ref stat date field.
 * 
 * @param date The date field from a @c struct @ref stat.
 * @return The month encoded in the date field.
 */
uint16_t constexpr statDateMonth(uint16_t date) {
	return (date >> 5) & 0b1111;
}

/**
 * Retrieves the day from a @c struct @ref stat date field.
 * 
 * @param date The date field from a @c struct @ref stat.
 * @return The day encoded in the date field.
 */
uint16_t constexpr statDateDay(uint16_t date) {
	return date & 0b11111;
}

/**
 * Retrieves the hour from a @c struct @ref stat time field.
 * 
 * @param time The time field from a @c struct @ref stat.
 * @return The hour encoded in the time field.
 */
uint16_t constexpr statTimeHour(uint16_t time) {
	return (time >> 11) & 0b11111;
}

/**
 * Retrieves the minute from a @c struct @ref stat time field.
 * 
 * @param time The time field from a @c struct @ref stat.
 * @return The minute encoded in the time field.
 */
uint16_t constexpr statTimeMinute(uint16_t time) {
	return (time >> 5) & 0b111111;
}

/**
 * Retrieves the second from a @c struct @ref stat time field.
 * 
 * Has a maximum resolution of 2 seconds.
 * 
 * @param time The time field from a @c struct @ref stat.
 * @return The second encoded in the time field.
 */
uint16_t constexpr statTimeSecond(uint16_t time) {
	return (time & 0b11111) * 2;
}

/**
 * Information about a file as retrieved from @ref fstat or @ref stat.
 * 
 * Dates and times are stored as a bitfield. The values they represent can be
 * accessed using the @ref statDateYear, @ref statDateMonth, @ref statDateDay,
 * @ref statTimeHour, @ref statTimeMinute, and @ref statTimeSecond helper
 * functions.
 *
 * Dates are stored in the following format:
 * <table>
 *     <tr>
 *         <td><b>Bit</b></td>
 *         <td><b>15</b></td>
 *         <td><b>14</b></td>
 *         <td><b>13</b></td>
 *         <td><b>12</b></td>
 *         <td><b>11</b></td>
 *         <td><b>10</b></td>
 *         <td><b>9</b></td>
 *         <td><b>8</b></td>
 *         <td><b>7</b></td>
 *         <td><b>6</b></td>
 *         <td><b>5</b></td>
 *         <td><b>4</b></td>
 *         <td><b>3</b></td>
 *         <td><b>2</b></td>
 *         <td><b>1</b></td>
 *         <td><b>0</b></td>
 *     </tr>
 *     <tr>
 *         <td><b>Value</b></td>
 *         <td colspan='7'>Year - 1980</td>
 *         <td colspan='4'>Month (indexed from 1)</td>
 *         <td colspan='5'>Day (indexed from 1)</td>
 *     </tr>
 * </table> 
 *
 * Times are stored in the following format:
 * <table>
 *     <tr>
 *         <td><b>Bit</b></td>
 *         <td><b>15</b></td>
 *         <td><b>14</b></td>
 *         <td><b>13</b></td>
 *         <td><b>12</b></td>
 *         <td><b>11</b></td>
 *         <td><b>10</b></td>
 *         <td><b>9</b></td>
 *         <td><b>8</b></td>
 *         <td><b>7</b></td>
 *         <td><b>6</b></td>
 *         <td><b>5</b></td>
 *         <td><b>4</b></td>
 *         <td><b>3</b></td>
 *         <td><b>2</b></td>
 *         <td><b>1</b></td>
 *         <td><b>0</b></td>
 *     </tr>
 *     <tr>
 *         <td><b>Value</b></td>
 *         <td colspan='5'>Hour</td>
 *         <td colspan='6'>Minute</td>
 *         <td colspan='5'>Second (divided by two, rounded down)</td>
 *     </tr>
 * </table>
 */
struct cas_stat {
	uint32_t unknown1;

	/**
	 * The size of the file, in bytes.
	 */
	uint32_t fileSize;

	/**
	 * The creation date of the file. For information on the storage format,
	 * see @c struct @ref stat.
	 */
	uint16_t creationDate;

	/**
	 * The creation time of the file. For information on the storage format,
	 * see @c struct @ref stat.
	 */
	uint16_t creationTime;

	/**
	 * The date the file was last modified. For information on the storage
	 * format, see @c struct @ref stat.
	 */
	uint16_t lastModifiedDate;

	/**
	 * The time the file was last modified. For information on the storage
	 * format, see @c struct @ref stat.
	 */
	uint16_t lastModifiedTime;

	uint16_t unknown2;

	/**
	 * The date the file was last accessed. For information on the storage
	 * format, see @c struct @ref stat.
	 */
	uint16_t lastAccessedDate;
};

/**
 * Information about a file/directory, as returned from @ref findFirst or
 * @ref findNext.
 */
struct findInfo {
	uint8_t unknown0[4];

	/// The type of entry which was located.
	enum : uint16_t {
		EntryTypeFile = 0x1,
		EntryTypeDirectory = 0x5
	} type;

	uint8_t unknown1[2];
	
	/**
	 * The size of the entry, in bytes. If the entry is a directory, @c size is
	 * zero.
	 */
	uint32_t size;
	
	uint8_t unknown2[8];
};

/**
 * Closes an open file.
 *
 * @param fd The file descriptor for the open file.
 * @return 0 on success, or a negative error code on failure.
 */
int cas_close(int fd);

/**
 * Closes a find handle.
 * 
 * Very, very, very bad things happen if a find handle is not closed.
 * 
 * @param findHandle The find handle to close.
 * @return 0 on success, or a negative error code on failure.
 */
int findClose(int findHandle);

/**
 * Starts a find operation, locating files matching a specific path.
 * 
 * Can be used to list the contents of a directory by using a wildcard. For
 * example, passing the path @c L"\\fls0\\*" or @c L"\\fls0\\*.*" matches all
 * files and directories on the calculator's flash (not recursive, though).
 * 
 * To find the next file/directory which matches the path, call @ref findNext,
 * passing in the find handle returned by this function. Ensure the find handle
 * is closed using @ref findClose when the find operation is finished. Bad
 * things happen if the handle is not closed.
 * 
 * @param[in] path The path to search. May contain wildcards.
 * @param[out] findHandle The find handle created. Must be closed when the find
 * operation is finished.
 * @param[out] name The name of the file/directory found.
 * @param[out] findInfoBuf Information about the found file.
 * @return 0 on success, or a negative error code on failure.
 */
int findFirst(const wchar_t *path, int *findHandle, wchar_t *name, struct findInfo *findInfoBuf);

/**
 * Returns information about the next matching file/directory in a find
 * operation.
 * 
 * @param findHandle The find handle returned from @ref findFirst.
 * @param[out] name The name of the file/directory found.
 * @param[out] findInfoBuf Information about the found file.
 * @return 0 on success, or a negative error code on failure.
 */
int findNext(int findHandle, wchar_t *name, struct findInfo *findInfoBuf);

/**
 * Retrieves information about an open file.
 *
 * @param fd The file descriptor of an open file to retrieve the information of.
 * @param[out] buf The retrieved information about the file.
 * @return 0 on success, or a negative error code on failure.
 */
int cas_fstat(int fd, struct stat *buf);

/**
 * Retrieves the memory address of a file.
 *
 * If the file is empty or the offset would point outside of the file,
 * @c EINVAL is returned.
 *
 * @param fd The file descriptor of an open file.
 * @param offset An offset to apply to the pointer to the file's data.
 * @param[out] addr The address of the file's data.
 * @return 0 on success, or a negative error code on failure.
 */
int getAddr(int fd, int offset, const void **addr);

/**
 * Repositions the file offset of the file descriptor. The new position depends
 * on the value of both @p offset and @p whence. See @ref lseek_whence_values.
 *
 * @param fd The file descriptor of an open file to change the file offset of.
 * @param offset The new offset, relative to some point.
 * @param whence Where @p offset is relative to.
 * @return The new file offset on success, or a negative error code on failure.
 */
int cas_lseek(int fd, int offset, int whence);

/**
 * Creates a directory.
 *
 * @param[in] path The path to the directory to be created.
 * @return 0 on success, or a negative error code on failure.
 */
int mkdir(const char *path);

/**
 * Opens a file on the file system.
 * 
 * The argument @p flags is fed a bitwise OR'd combination of flags. See
 * @ref open_flags_values.
 *
 * @param[in] path The path to the file to open.
 * @param flags A bitfield describing the mode in which to open the file.
 * @return A file descriptor on success, or a negative error code on failure.
 */
int cas_open(const char *path, int flags);

/**
 * Reads up to @c count bytes from a file, and stores them in @c buf.
 *
 * If @c count bytes cannot be read from the file, as many bytes as possible are
 * read (i.e. between 0 and @c count - 1 bytes are read and stored in @c buf).
 *
 * @param fd The file descriptor of an open file to read from.
 * @param[out] buf A buffer to place the read bytes into.
 * @param count The maximum number of bytes to read.
 * @return The number of bytes read on success, or a negative error code on
 * failure.
 */
int cas_read(int fd, void *buf, int count);

/**
 * Deletes a file or directory.
 *
 * @param[in] path The path to the file or directory to be deleted.
 * @return 0 on success, or a negative error code on failure.
 */
int remove(const char *path);

/**
 * Renames a file or directory.
 *
 * @param[in] oldPath The path to the file or directory to be renamed.
 * @param[in] newPath The path to the new name for the file or directory.
 * @return 0 on success, or a negative error code on failure.
 */
int rename(const char *oldPath, const char *newPath);

/**
 * Retrieves information about a file.
 *
 * @param[in] path The path to the file to retrieve the information of.
 * @param[out] buf The retrieved information about the file.
 * @return 0 on success, or a negative error code on failure.
 */
int stat(const char *path, struct stat *buf);

/**
 * Writes @c count bytes from @c buf to a file.
 *
 * @param fd The file descriptor of an open file to write to.
 * @param[in] buf A buffer of bytes to write to the file.
 * @param count The number of bytes from @c buf to write to the file.
 * @return The number of bytes written on success, or a negative error code on
 * failure.
 */
int cas_write(int fd, const void *buf, int count);
