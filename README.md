Данная программа предназначена для проверки на антиплагиат программ с расширением .с.

Чтобы скомпилить:
  make

Запуск:
  ./start file1 file2|dirname [-f]

Вывод man:
  ./start

*Примечания:

При введении параметров может быть введено file1 file2 или file1 dirname 
причём при вводе дирректории файл будет сравнён со всеми файлами внутри него.

Флаг -f служит для сравнения файлов file1 file2 && file2 file1, а затем выводит средний процент плагиата.