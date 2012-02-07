//! \file d3l.h D3 Library head file
//! \brief D3 Liberary Version 2011.12.23

// Define version number of the library file
#ifndef d3l_version
#define d3l_version 5

// Include required *standard* C headers.
#include <dirent.h>     // DIR..
#include <sys/time.h>   // time_t,time(),gettimeofday()...
#include <fcntl.h>      // O_WRONLY|O_CREAT..

//! Define warning mode.
#define D3L_NO_WARNING
//! Define sys timing variable define mode.
#define D3L_SYS_TIME_VAR
//! Define gb code mode.
#define D3L_GB_CODE "GBK"
//! Define log file path.
#define D3L_LOG_FILE "d3l.log"

// Define for standard c.
#ifdef __cplusplus
extern "C"{
#endif

////////////////////////////////////////////////////////////////////////
// System Operation
////////////////////////////////////////////////////////////////////////

//! Print a system error information.
void d3l_sys_err(const char *, const char * = D3L_LOG_FILE);

//! Get system time.
int d3l_sys_time(char **);

#ifdef D3L_SYS_TIME_VAR
    //! Timing variable define.
    extern struct timeval d3l_tv_start;
    extern struct timezone d3l_tz_start;

    //! Timing begin.
    int d3l_sys_time_begin(struct timeval * = &d3l_tv_start, 
            struct timezone * = &d3l_tz_start);

    //! Timing up.
    int d3l_sys_time_up(char **, struct timeval * = &d3l_tv_start, 
            struct timezone * = &d3l_tz_start);
#else
    //! Timing begin.
    int d3l_sys_time_begin(struct timeval *, struct timezone *);

    //! Timing up.
    int d3l_sys_time_up(char **, struct timeval *, struct timezone *);
#endif

////////////////////////////////////////////////////////////////////////
// File Operation
////////////////////////////////////////////////////////////////////////

//! Return a file access stat.
int d3l_fop_access(const char *, int);

//! Remove a file from disk.
int d3l_fop_remove(const char *);

//! Get line number of file.
int d3l_fop_linenum(const char *);

//! Get the size of file.
int d3l_fop_size(const char *sz_file);

// Define for standard c.
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// Include required *standard* C++ headers.
#include <string>

//! Write a file using write().
/*!
  \brief Write a file from a buff using ifstream.
  \param[in] sz_file file path.
  \param[in] buff memory point to input.
  \param[in] size input memory size.
  \retval ==0 Successed; <0 Failed.
 */
template <class T>
int d3l_fop_write(const char *sz_file, T* buff, const unsigned int size)
{
    if(access(sz_file, F_OK) == 0)
    {
        std::string str_err;
        if(0 == unlink(sz_file))
        {
            str_err = "Note d3l::d3l_fop_write(const char *sz_file, T* buff, const unsigned int size) File ";
            str_err = str_err + sz_file + " has been delete!";
        }
        else
        {
            str_err = "ERROR d3l::d3l_fop_write(const char *sz_file, T* buff, const unsigned int size) File ";
            str_err = str_err + sz_file + " can't been delete!";
        }
        d3l_sys_err(str_err.c_str());
    }

    int fout = open(sz_file, O_WRONLY|O_CREAT, 0777);
    ssize_t rs = write(fout, buff, size * sizeof(T));
    close(fout);
    return rs;
}

//! Read a file using read().
/*!
  \brief Write a file from a buff using ifstream.
  \param[in] sz_file file path.
  \param[in] buff memory point to input.
  \retval ==0 Successed; <0 Failed.
 */
template <class T>
int d3l_fop_read(const char *sz_file, T *&buff)
{
    if(access(sz_file, F_OK) != 0)
    {
        std::string str_err = "ERROR d3l::d3l_fop_read(const char *sz_file, T* buff) File ";
        str_err = str_err + sz_file + " can't be found!";
        d3l_sys_err(str_err.c_str());
        return -1;
    }

    ssize_t file_size = d3l_fop_size(sz_file);
    int fin = open(sz_file, O_RDONLY, 0777);
    ssize_t sum = 0;
    ssize_t len = 0;
    T *buff_tmp = NULL;
    d3l_mem_create(buff, file_size/sizeof(T), static_cast<T>(0));
    d3l_mem_create(buff_tmp, 1024, static_cast<T>(0));

    while((len = read(fin, buff_tmp, 1024)) > 0)
    {
        memcpy(buff + sum/sizeof(T), buff_tmp, len);
        sum += len;
    }

    d3l_mem_free(buff_tmp);
    close(fin);

    return sum;
}

#endif // #ifdef __cplusplus


// Define for standard c.
#ifdef __cplusplus
extern "C"{
#endif
////////////////////////////////////////////////////////////////////////
// Dirent Operation
////////////////////////////////////////////////////////////////////////

//! Open dirent.
int d3l_dop_open(DIR **, const char *);

//! Close dirent.
int d3l_dop_close(DIR **);

//! Get file number of dirent.
int d3l_dop_filenum(const char *);

//! Create dirent by path.
int d3l_dop_create(const char *);

////////////////////////////////////////////////////////////////////////
// Charset Operation
////////////////////////////////////////////////////////////////////////

//! Code convert from one to another.
int d3l_charset_code_convert(char *from_charset, char *to_charset, char *inbuf, int inlen, char *outbuf, int outlen);

//! Code convert string from UNICODE to GB2312.
int d3l_charset_u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

//! Code convert string from GB2312 to UNICODE.
int d3l_charset_g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

//! Print char content using 16bit format.
int d3l_charset_printc(const char *cc);

// Define for standard c.
#ifdef __cplusplus
}
#endif

// Define the interfaces only for c++.
#ifdef __cplusplus

// Include required *standard* C++ headers.
#include <sstream>
#include <fstream>
//using namespace std;
#include <memory.h>

////////////////////////////////////////////////////////////////////////
// Define template function.
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Convert Operation
////////////////////////////////////////////////////////////////////////

//! Convert a std::string to other type.
/*!
  \brief Convert a std::string to other type using stringstream.
  \param[in]  s_str string.
  \param[out] value string value (int, double and so on...).
 */
template <class T>
void d3l_convert_from_string(const std::string &s_str, T &value)
{
    std::stringstream ss(s_str);
    ss >> value;
}

//! Convert an other type value to std::string.
/*!
  \brief Convert a std::string to other type using stringstream.
  \param[in] value string value (int, double and so on...).
  \param[out]  s_str string.
 */
template <class T>
void d3l_convert_to_string(const T &value, std::string &s_str)
{
    std::stringstream ss;
    ss << value;
    ss >> s_str;
}

////////////////////////////////////////////////////////////////////////
// Memory Operation
////////////////////////////////////////////////////////////////////////

//! Create memory space and set to default value.
/*!
  \brief Create memory space and set to default value using <memory.h>.
  \param[in]  p_ptr point.
  \param[in]  size memory size.
  \param[in]  value default value(default is 0).
  \retval memory point.
 */
template <class T>
T* d3l_mem_create(T *&p_ptr, const unsigned int &size, T value = 0)
{
    p_ptr = new T[size];
    if(NULL == p_ptr)
    {
        std::string err_str = "ERROR d3l::T *d3l_mem_create(const unsigned int, const int = 0) ";
        d3l_sys_err(err_str.c_str());
    }
    else
        memset(p_ptr, value, size * sizeof(T));
    return p_ptr;
}

//! Free memory space.
/*!
  \brief Free memory space using delete.
  \param[in]  p_ptr point.
  \retval memory point.
 */
template <class T>
T* d3l_mem_free(T *&p_ptr)
{
    if(NULL != p_ptr)
    {
        delete[] p_ptr;
        p_ptr = NULL;
    }
    return p_ptr;
}

////////////////////////////////////////////////////////////////////////
// Charset Operation
////////////////////////////////////////////////////////////////////////

//! Code convert string object from UNICODE to GB2312.
int d3l_charset_u2g(const std::string &in_str, std::string &out_str);

//! Code convert string object from GB2312 to UNICODE.
int d3l_charset_g2u(const std::string &in_str, std::string &out_str);

////////////////////////////////////////////////////////////////////////
// STL String Object Operation
////////////////////////////////////////////////////////////////////////

//! Erase sub string from string object.
int d3l_str_erase(std::string& str, const std::string& sub_str);

//! Replace sub string from string object.
int d3l_str_replace(std::string &str, const std::string &str_src, const std::string &str_dest);

////////////////////////////////////////////////////////////////////////
// File Operation
////////////////////////////////////////////////////////////////////////

//! Open a file by ifstream.
int d3l_fop_open_ifstream(std::ifstream &, const char *);

#endif // #ifdef __cplusplus

#endif // #ifndef d3l_version
