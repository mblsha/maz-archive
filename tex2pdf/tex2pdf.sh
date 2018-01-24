#!/bin/bash
#
# Маленький скриптик для упрощения конвертирования TeX'ов в PDF'ки
#
# Для работы нужен teTeX + дополнительные теховые тулзы и tcilatex.tex

if [ $1 ]
then
   tex_conv -T -f1251 -iu $1 -ob temp_file.tex
	pdflatex temp_file.tex
   
   mv temp_file.pdf $1.pdf

	rm -f temp_file.tex
	rm -f temp_file.aux
	rm -f temp_file.log
else
	echo
	echo "Синтаксис:"
   echo "   tex2pdf <имя_файла.tex>"
   echo
   echo "   Конвертирует .tex файл в .pdf"
   echo
fi
