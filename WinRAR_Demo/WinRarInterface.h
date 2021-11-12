#ifndef WINRARINTERFACE_H
#define WINRARINTERFACE_H

namespace Pan
{

        /**
         * @brief WinRar软件的接口
         */
        class WinRarInterface
        {

        private:
                static const unsigned int bufferSize = 256;

                /**
         * @brief 内部使用的字符串复制函数
         * @param dest
         * @param src
         * @return
         */
        private:
                static unsigned int _string_copy(wchar_t *dest, const wchar_t *src);

        private:
                /**
                 * @brief 判断指定的路径是否一个存在的文件
                 * @param path 指定的路径
                 * @return bool
                 */
                static bool is_existing_file(const wchar_t *path);

        private:
                /**
                 * @brief 判断指定的路径是否一个存在的目录
                 * @param path 指定的路径
                 * @return bool
                 */
                static bool is_existing_directory(const wchar_t *path);

        public:
                /**
                 * @brief 判断给定的目录是否为一个WinRAR目录
                 * @param path 给定的目录
                 * @return bool
                 */
                static bool is_winrar_directory(const wchar_t *path);

        public:
                static WinRarInterface *autoDetect();

        public:
                static WinRarInterface *byInstallDirectory(const wchar_t *install_directory);

                /**
         * @brief 构造函数, 不允许外部构造对象
         * @param winrar_directory_fullpath 指定WinRar的安装目录
         */
        private:
                WinRarInterface(const wchar_t *winrar_directory_fullpath);

                /**
        * @brief 析构函数, 释放内存
        */
        public:
                ~WinRarInterface();

        private:
                /**
     * @brief 保存WinRar的安装路径
     */
                wchar_t *m_winRarDirectory;

        private:
                bool _rar_toZip(const wchar_t *zipFile,
                                const wchar_t *path,
                                wchar_t *buffer1024_cmdline = nullptr,
                                const wchar_t *workingDir = nullptr);

                /**
         * @brief 获取该接口使用的WinRar的目录
         * @return 该接口使用的WinRar的目录
         */
        public:
                const wchar_t *winRarDirectory() { return this->m_winRarDirectory; }

                /**
        * @brief 使用winrar.exe解压文件到指定的文件夹下, 目录结构保持不变
        * @param zipFile
        * @param directory
        * @return
        */
        public:
                bool winRarUnZip(const wchar_t *zipFile, const wchar_t *directory);

                /** 使用rar.exe将一个文件夹下的所有文件夹及文件压缩进压缩包
        * @brief toZip
        * @param zipFile 指定压缩包的路径
        * @param direcotory_fullpath 文件夹的绝对路径
        * @param buffer_findStr 保留，可以为null
        * @return
        */
        public:
                bool rarZipDirectory //
                    (
                        const wchar_t *zipFile,
                        const wchar_t *directory_path,
                        wchar_t *buffer_findStr = nullptr,
                        bool (*select)(const wchar_t *name, const wchar_t *directory) = nullptr //
                    );

        public:
                /**
                 * @brief 添加指定的条目到指定的压缩文件当中，此函数可以用于创建压缩文件
                 * @param compressed_file 指定的压缩文件的路径
                 * @param item 要压缩的条目，可以是文件或者是一个文件夹
                 * @param current_directory 指定 WinRAR 的运行目录，如果不指定，则使用程序的当前目录
                 * @param args 额外的参数
                 * @return winrar 退出码
                 */
                int winrar_add //
                    (
                        const wchar_t *compressed_file,
                        const wchar_t *item,
                        const wchar_t *current_directory = nullptr,
                        const wchar_t *args = nullptr //
                    );
        };
};
#endif // WINRARINTERFACE_H
