#!/bin/bash
#
# ��������� �������� ��� ��������� ��������������� TeX'�� � PDF'��
#
# ��� ������ ����� teTeX + �������������� ������� ����� � tcilatex.tex

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
	echo "���������:"
   echo "   tex2pdf <���_�����.tex>"
   echo
   echo "   ������������ .tex ���� � .pdf"
   echo
fi
