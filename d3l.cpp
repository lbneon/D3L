//! \file d3l.cpp D3 Library cpp file
//! \brief D3 Liberary Version 2011.12.23

// Include required *standard* C++ headers.
#include <iostream>
#include <fstream>
#include <iconv.h>
//using namespace std;
#include <stdlib.h>   // free..
#include <sys/stat.h> // struct stat, struct statbuf..
#include "d3l.h"

#define RWRWRW  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
#define RWXRXRX  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

//! Print a system error information.
/*!
  \brief Print a system error information on screen.
  \param[in] sz_ptr error information.
  \param[in] sz_file error output file path.
 */
void d3l_sys_err(const char *sz_err, const char *sz_file)
{
    char *sz_time = NULL;
    d3l_sys_time(&sz_time);
#ifdef D3L_NO_WARNING
    std::ofstream log(sz_file, std::ios_base::app);
    std::streambuf *old_buf = std::cout.rdbuf(log.rdbuf());
#endif
    std::cout << sz_time << std::endl << sz_err << std::endl;
#ifdef D3L_NO_WARNING
    std::cout.rdbuf(old_buf);
#endif
    d3l_mem_free(sz_time);
    return;
}

//! Get system time.
/*!
  \brief Get system time using time();
  \param[out] psz_time time string to out.
  \retval ==0 Successed; <0 Failed.
 */
int d3l_sys_time(char **psz_time)
{
    if(NULL != *psz_time)
    {
        d3l_mem_free(*psz_time);
        *psz_time = NULL;
    }
    d3l_mem_create(*psz_time, 35);

    time_t timer;
    struct tm* t_tm;
    time(&timer);
    t_tm = localtime(&timer);
    sprintf(*psz_time, "D3L::Time:%4d-%02d-%02d %02d:%02d:%02d", t_tm->tm_year+1900,
            t_tm->tm_mon+1, t_tm->tm_mday, t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);
    return 0;
}

#ifdef D3L_SYS_TIME_VAR
//! Timing variable define.
struct timeval d3l_tv_start;
struct timezone d3l_tz_start;
// Timing print
// Example: char *psz_time = NULL;
//          d3l_sys_time_begin();
//          d3l_sys_time_up(&psz_time);
//          printf("Timing print=%s \n",psz_time);
#endif

//! Timing begin.
/*!
  \brief Timing begin using gettimeofday();
  \param[in] ptv_start timeval point of timing beginning.
  \param[in] ptz_start timezone point of timing beginning.
  \retval ==0 Successed; <0 Failed.
 */
int d3l_sys_time_begin(struct timeval *ptv_start, struct timezone *ptz_start)
{
    //! start time
    gettimeofday(ptv_start, ptz_start);
    return 0;
}

//! Timing up.
/*!
  \brief Timing up using gettimeofday();
  \param[out] psz_time time string to out.
  \param[in] ptv_start timeval point of timing beginning.
  \param[in] ptz_start timezone point of timing beginning.
  \retval ==0 Successed; <0 Failed.
 */
int d3l_sys_time_up(char **psz_time, struct timeval *ptv_start, 
        struct timezone  *ptz_start)
{
    //! end time
    struct timeval tv_end;
    struct timezone tz_end;
    gettimeofday(&tv_end, &tz_end);

    if(NULL != *psz_time)
    {
        d3l_mem_free(*psz_time);
        *psz_time = NULL;
    }
    d3l_mem_create(*psz_time, 50);
    //! compute time
    sprintf(*psz_time, "D3l:Cost time: %8ld sec %8ld usec\n", 
            tv_end.tv_sec - ptv_start->tv_sec,tv_end.tv_usec-ptv_start->tv_usec);
    return 0;
}

//! Return a file access stat.
/*!
  \brief Return a file access stat using access function.
  \param[in] sz_file file path.
  \param[in] mode R_OK(Read) W_OK(Write) X_OK(Execute) F_OK(Exist).
  \retval ==0 Successed; <0 Failed.
 */
int d3l_fop_access(const char *sz_file, int mode)
{
    if(access(sz_file, R_OK) < 0)
    {
        std::string str_err = "ERROR d3l::d3l_fop_access(const char *, int) File ";
        str_err = str_err + sz_file + " access error!";
        d3l_sys_err(str_err.c_str());
        return -1;
    }

    return 0;
}

//! Remove a file from disk.
/*!
  \brief Remove a file from disk using unlink command.
  \param[in] sz_file file path.
  \retval ==0 Successed; <0 Failed.
 */
int d3l_fop_remove(const char *sz_file)
{
    if(unlink(sz_file) < 0)
    {
        std::string str_err = "ERROR d3l::d3l_fop_remove(const char *) File ";
        str_err = str_err + sz_file + " can't be removed!";
        d3l_sys_err(str_err.c_str());
        return -1;
    }
    return 0;
}

//! Get line number of file.
/*!
  \brief Get line number of file by open the file.
  \param[in] sz_file file path.
  \retval >=0 Line num; <0 Failed.
 */
int d3l_fop_linenum(const char *sz_file)
{
    std::ifstream ifs;
    ifs.open(sz_file);
    if(ifs.fail())
    {
        std::string str_err = "ERROR d3l::d3l_fop_linenum(const char *) File ";
        str_err = str_err + sz_file + " can't be removed!";
        d3l_sys_err(str_err.c_str());
        return -1;
    }

    int num = 0;
    std::string str_line;
    while(getline(ifs, str_line))
        num++;

    return num;
}

//! Get the size of file.
/*!
  \brief Get the size of file by open the file.
  \param[in] sz_file file path.
  \retval >=0 File size; <0 Failed.
 */
int d3l_fop_size(const char *sz_file)
{
    FILE *fp = fopen(sz_file,"r");
    fseek(fp,0L,SEEK_END);
    int size = ftell(fp);
    fclose(fp);
    return size;
}

//! Open a file using ifstream.
/*!
  \brief Open a file using ifstream.
  \param[in] ifstream infile stream.
  \param[in] sz_file file path.
  \retval ==0 Successed; <0 Failed.
 */
int d3l_fop_open_ifstream(std::ifstream &ifs_in, const char *sz_file)
{
    ifs_in.open(sz_file);
    if(ifs_in.fail())
    {
        std::string str_err = "ERROR d3l::d3l_fop_open_ifstream(ifstream &, char *) File ";
        str_err = str_err + sz_file + " can't be opened!";
        d3l_sys_err(str_err.c_str());
        return -1;
    }
    return 0;
}

//! Code convert from one to another.
/*!
  \brief Code convert from one to another using iconv.
  \param[in] from_charset src code charset.
  \param[in] to_charset dest code charset.
  \param[in] inbuf src string.
  \param[in] inlen src string length.
  \param[out] outbuf dest string.
  \param[in] outlen dest string length.
  \retval ==0 Successed; <0 Failed.
 */
int d3l_charset_code_convert(char *from_charset, char *to_charset, char *inbuf, int inlen, char *outbuf, int outlen)
{
    iconv_t cd;
    char **pin = &inbuf;
    char **pout = &outbuf;

    cd = iconv_open(to_charset, from_charset);
    if (0 == cd)
    {
        std::string str_err = "ERROR d3l::int d3l_charset_code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen) \n iconv_open(to_charset, from_charset)";
        d3l_sys_err(str_err.c_str());
        return -1;
    }
    memset(outbuf, 0, outlen);
    if (-1 == static_cast<int>(iconv(cd, pin, reinterpret_cast<size_t *>(&inlen), pout, reinterpret_cast<size_t *>(&outlen))))
    //if (-1 == iconv(cd, pin, &inlen, pout, &outlen))
    {
        std::string str_err = "ERROR d3l::int d3l_charset_code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen) \n iconv(cd, pin, &inlen, pout, &outlen)";
        d3l_sys_err(str_err.c_str());
        return -1;
    }
    iconv_close(cd);
    return 0;
}

//! Code convert string from UNICODE to GB2312.
/*!
  \brief Code convert string from UNICODE to GB2312 using iconv.
  \param[in] inbuf src string.
  \param[in] inlen src string length.
  \param[out] outbuf dest string.
  \param[in] outlen dest string length.
  \retval ==0 Successed; <0 Failed.
 */
// UNICODE code to GB2312 code
// Example: char *in_utf8 = "utf8字符串";
//          rec = u2g(in_utf8,strlen(in_utf8),out,OUTLEN);
//          printf("unicode-->gb2312 out=%s\n",out);
int d3l_charset_u2g(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
    //return d3l_charset_code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);
    return d3l_charset_code_convert(const_cast<char *>("utf-8"),const_cast<char *>(D3L_GB_CODE),inbuf,inlen,outbuf,outlen);
}

//! Code convert string from GB2312 to UNICODE.
/*!
  \brief Code convert string from GB2312 to UNICODE using iconv.
  \param[in] inbuf src string.
  \param[in] inlen src string.
  \param[out] outbuf dest string.
  \param[in] outlen dest string length.
  \retval ==0 Successed; <0 Failed.
 */
// GB2312 code to UNICODE code
// Example: char *in_gb2312 = "\xbe\xb2\xcc\xac\xc4\xa3\xca\xbd";
//          rec = g2u(in_gb2312,strlen(in_gb2312),out,OUTLEN);
//          printf("gb2312-->unicode out=%s \n",out);
int d3l_charset_g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
    //return d3l_charset_code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
    return d3l_charset_code_convert(const_cast<char *>(D3L_GB_CODE),const_cast<char *>("utf-8"),inbuf,inlen,outbuf,outlen);
}

//! Code convert string object from UNICODE to GB2312.
/*!
  \brief Code convert string object from UNICODE to GB2312 using iconv.
  \param[in] str_in src string object.
  \param[out] str_out dest string object.
  \retval ==0 Successed; *<0 Failed.*
 */
int d3l_charset_u2g(const std::string &str_in, std::string &str_out)
{
    char *sz_in = NULL, *sz_out = NULL;
    size_t buf_size = (str_in.length() + 1) * 2;
    sz_in = strdup(str_in.c_str());
    d3l_mem_create(sz_out, buf_size);
    d3l_charset_code_convert(const_cast<char *>("utf-8"),const_cast<char *>(D3L_GB_CODE),sz_in,str_in.length(),sz_out,buf_size);
    str_out = sz_out;
    free(sz_in);
    d3l_mem_free(sz_out);
    return 0;
}

//! Code convert string object from GB2312 to UNICODE.
/*!
  \brief Code convert string object from GB2312 to UNICODE using iconv.
  \param[in] str_in src string object.
  \param[out] str_out dest string object.
  \retval ==0 Successed; *<0 Failed.*
 */
int d3l_charset_g2u(const std::string &str_in, std::string &str_out)
{
    char *sz_in = NULL, *sz_out = NULL;
    size_t buf_size = (str_in.length() + 1) * 20;
    sz_in = strdup(str_in.c_str());
    d3l_mem_create(sz_out, buf_size);
    d3l_charset_code_convert(const_cast<char *>(D3L_GB_CODE),const_cast<char *>("utf-8"),sz_in,str_in.length(),sz_out,buf_size);
    str_out = sz_out;
    free(sz_in);
    d3l_mem_free(sz_out);
    return 0;
}

//! Print char content using 16bit format.
/*!
  \brief Print char content using 16bit format, using cout.
  \param[in] ch src string.
  \retval ==0 Successed; *<0 Failed.*
 */
int d3l_charset_printc(const char *sz_c)
{
    char *ch = const_cast<char *>(sz_c);
    std::cout << "Hex Char Code:" << std::endl;
    do
        std::cout << std::hex << static_cast<unsigned int>(*ch++) << " ";
    while(*ch != '\0');
    std::cout << std::endl;
    return 0;
}

//! Open dirent.
/*!
  \brief Open dirent using opendir().
  \param[out] dpp DIR point point.
  \param[in] sz_dir dir path.
  \retval ==0 Successed; <0 Failed.
 */
int d3l_dop_open(DIR **dpp, const char *sz_dir)
{
    struct stat statbuf;
    //! Check dirent status.
    if (lstat(sz_dir, &statbuf) < 0)
    {
        std::string str_err = sz_dir;
        str_err = "ERROR: Dir " + str_err + " stat error!";
        d3l_sys_err(str_err.c_str());
        return -1;
    }
    if (S_ISDIR(statbuf.st_mode) == 0)
    {
        std::string str_err = sz_dir;
        str_err = "ERROR: " + str_err + " is not a directory!";
        d3l_sys_err(str_err.c_str());
        return -2;
    }
    if(NULL == (*dpp = opendir(sz_dir)))
    {
        std::string str_err = sz_dir;
        str_err = "ERROR: Can't open dir " + str_err + " !";
        d3l_sys_err(str_err.c_str());
        return -3;
    }
    return 0;
}

//! Close dirent.
/*!
  \brief Close dirent using closedir().
  \param[out] dpp DIR point point.
  \retval ==0 Successed; <0 Failed.
 */
int d3l_dop_close(DIR **dpp)
{
    if(closedir(*dpp) < 0)
    {
        std::string str_err = "ERROR: Can't close dirent!";
        d3l_sys_err(str_err.c_str());
        return -1;
    }
    return 0;
}

//! Get file number of dirent.
/*!
  \brief Get file number of dirent using D3L.
  \param[in] sz_dir dir path.
  \retval >=0 File num; <0 Failed.
 */
int d3l_dop_filenum(const char *sz_dir)
{
    struct dirent *dirp;
    DIR *dp;
    if(d3l_dop_open(&dp, sz_dir) < 0)
        return -1;

    int num = 0;
    //! 循环读取目录中文件
    while(NULL != (dirp = readdir(dp)))
    {
        if(0 == strcmp(dirp->d_name, ".") ||
           0 == strcmp(dirp->d_name, "..") )
            continue;
        num++;
    }
    return num;
}

//! Create dirent by path.
/*!
  \brief Create dirent by path using makedir.
  \param[in] sz_dir dir path.
  \retval ==0 Successed; <0 Failed.
 */
int d3l_dop_create(const char *sz_dir)
{
    char *dirname = NULL;
    int pos = 0;
    d3l_mem_create(dirname, strlen(sz_dir) + 1, '\0');
    while('\0' != sz_dir[pos])
    {
        dirname[pos] = sz_dir[pos];
        if('/' == sz_dir[pos] || strlen(sz_dir) - 1 == pos)
        {
            if (access(dirname,F_OK)<0)
            {
                if (mkdir(dirname, RWXRXRX)<0)
                {
                    std::string str_err = "ERROR: d3l:: int d3l_dop_create(const char *) Can't create dirent.";
                    d3l_sys_err(str_err.c_str());
                }
                else
                {
                    std::string str_err = "ERROR: d3l:: int d3l_dop_create(const char *) ";
                    str_err += dirname;
                    str_err += "has been created.";
                    d3l_sys_err(str_err.c_str());
                }
            }
            else
            {
                std::string str_err = "ERROR: d3l:: int d3l_dop_create(const char *) ";
                str_err += dirname;
                str_err += "is already exist.";
                d3l_sys_err(str_err.c_str());
            }
        }
        pos++;
    }
    d3l_mem_free(dirname);
    return 0;
}

//! Erase sub string from string object.
/*!
  \brief Erase sub string from string object using string.erase();
  \param[out] str string object to erase.
  \param[in] sub_str sub string.
  \retval ==0 Successed; <0 Failed.
 */
int d3l_str_erase(std::string &str, const std::string &sub_str)
{
    std::string::size_type idx = 0;
    while(std::string::npos != (idx = str.find(sub_str, 0)))
    {
        str.erase(idx,sub_str.size());
    }
    return 0;
}

//! Replace sub string from string object.
/*!
  \brief Replace sub string from string object using string.replace();
  \param[out] str string object to replace.
  \param[in] str_src src sub string.
  \param[in] str_dest dest sub string.
  \retval ==0 Successed; <0 Failed.
 */
int d3l_str_replace(std::string &str, const std::string &str_src, const std::string &str_dest)
{
    std::string::size_type idx = 0;
    while(std::string::npos != (idx = str.find(str_src, idx)))
    {
        str.replace(idx, str_src.size(), str_dest);
        idx = idx + str_dest.size();
    }
    return 0;
}
