  ______   ___    ___
 /\  _  \ /\_ \  /\_ \
 \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
  \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
   \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
    \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
     \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
                                    /\____/
                                    \/___/
         ______     ___               ______      __        __
        /\__  _\__ /\_ \             /\  ___\    /\ \  __  /\ \__
        \/_/\ \/\_\\//\ \      __    \ \ \__/    \_\ \/\_\/\__  _\  ___   _ __
           \ \ \/_/_ \ \ \   /'__'\   \ \  __\   / __ \/_/\/_/\ \/ / __'\/\''__\
            \ \ \/\ \ \_\ \_/\  __/    \ \ \_/__/\ \L\ \/\ \ \ \ \/\ \L\ \ \ \/
             \ \_\ \_\/\____\ \____\    \ \_____\ \_____\ \_\ \ \_\ \____/\ \ \
              \/_/\/_/\/____/\/____/     \/_____/\/_____/\/_/  \/_/\/___/  \/_/

                                   от MAZsoft

********************************************************************************

  Содержание
    - Изменения
    - Что такое ATED ?
    - Особенности
    - Использование
      - Подготовка тайлов
      - Редактирование карты
      - Управление
    - Обратная связь
    - Авторы

  Изменения
    0.1 beta		Первый выпуск

  Что такое ATED ?
    ATED - это двужмерный редактор тайл-карт. Кто не знает, могу
    сказать что это прямоугольный массив информации, которая
    обычно является порядковыми номерами тайлов. 

    ТАЙЛ (англ. плитка) - в данном случае является квадратным
    изображением, укладываемом друг рядом с другом. Получается
    большое изображение, сложенное из маленьких кусочков.
    Этот метод позволяет сэкономить память, а также облегчИть
    ATED is 2D tile editor, it can edit tile maps. For starters: tile map is a
    rectangular array of data. Data usually is a tile number.

  Особенности
    - Очень удобное и простое редактирование карт
    - Автоматически все сохраняет при выходе и загружает при загрузке,
      Это действительно круто :-)
    - Карты ATED могут содержать 3 слоя: Задний (Back), Передний (Fore)
      и Дополнительно-Информационный (Info)
    - Просмотр каждого слоя может быть включен или выключен
    - Раздельный просмотр и редактирование слоев (Например: видишь back, fore
      и info, а редактируешь только back)
    - Мышка интегрирована в процесс редактирования -- левой кнопкой ставишь
      тайлы, а правой хватаешь
    - Функция Отмены
    - Copy & Paste, Копировать можно прямо с окошка TileSelect
    - А еще Snap-Paste Mode
    - Заливка, в т.ч. КОПИРОВАННЫМИ тайлами
    - Заливка прямоугольной области
    - Функция поиска тайлов
    - Сетка
    - Полностью переписанный Графический Интерфейс Пользователя Allegr'ы
    - Поддерживает кучу разрешений
    - Читает и пишет в файлы, сжатые по методу LZSS
    - Изменяемый фон :-)
 

  Использование
    Готовим Тайлы
      Во первых я скажу что поддерживаются только КВАДРАТНЫЕ тайлы. Это
      значит, что вы можете использовать тайлы размером 8х8, 16х16, 32х32
      и т.д. По идее должны работать все тайлы, если они квадратные. Я
      рекомендую использовать размеры по степеням двойки (2^3 = 8; 2^4 = 16;
      2^5 = 32; ...). Еще размеры должны быть больше 8. Если что не будет
      работать, тогда пишите -- попробую помочь.

      Во вторых, тайлы для всех слоев различны. Тайлы хранятся в обычных
      Аллегровских датафайлах, только у них специальная внутренняя
      организация. Первые три пункта в этих файлах -- под-датафайлы, в
      которых, собственно и содержатся тайлы. В первом -- для фона, во
      втором -- для переднего плана, и в третьем -- иконки. Все тайлы
      должны быть одного размера и глубины цвета. Четвертым пунктом может
      быть палитра, если файлы восьмибитные, но не обязательна.

      Third, easiest way for creating TileGFX files is to use GRABBER utility,
      which is part of ALLEGRO. Step-by-step example:
        1) Enter GRABBER
        2) Open "gfx.dat" file, you can found it into "ATED\TILES" folder
        3) Use "Read bitmap" menu command from GRABBER, and load 
           "back_tiles.pcx", which also can be found into "ATED\TILES"
        4) Make sure, you selected "BACKGROUND TILES" at the left!
        5) Use "Grab from grid". Set X-size and Y-size to 16x16, type to
           "RLE SPRITE" and "Cols" to "256". Click "OK"
        6) Select "FOREGROUND TILES" and repeat steps 4-5 with bitmap
           "fore_tiles.pcx"
        7) Select "INFO TILES" and repeat steps 4-5 with bitmap "info_tiles.pcx"

    Editing TileMap
      1) Create new tile map: from "File" menu, select "Manage Maps", click
         "Create", give it name and dimensions.
      2) It's time to allocate some memory onto layers:
         "File" -> "Manage Palnes". Select all 3 checkboxes, click "Done",
         confirm your action.
      3) You need to load tileGFX file now: "File" -> "Load TileGFX file", and
         select some file.
      4) Now you can select tiles by pressing SPACEBAR: Press it select some
         tiles for back, fore and info. Press SPACE
      5) Firstly, press 4, 5, 6 to turn on viewing layers. Now press 1, 2, 3 to
         turn on edit mode on them.
      6) Move mouse around and press buttons, you would see, what happens :-)

    Controls
      KEYBOARD
        CTRL-X			Quit
        F9			Select video mode
        C			Copy Mode
        P	 		Paste Mode
        B			Blockfill mode
        F			Floodfill mode
				NOTE: If you press RIGHT mouse button, or
                                CTRL+LEFT mouse button, it will cause filling
                                with that, you have in copy buffer
	U			Undo
        T			Tilesearch
	G			Toggle Grid mode
	S			Toggle Snap-Paste mode (only work in Paste mode)
	F3			Toggle Paste overlay
	I			Map statistics
   	F12			Take screenshot (ated_XX.pcx)
	ENTER			Normally, change value for info plane, but in
                                Copy mode it selects, in Blockfill fills :-)

  Feedback
    If you find some bugs in a program, have some interesting ideas,
    then please send me email. You can find it in the end.


  Credits    
    Programming skills and some art - Michail Pichagin aka MAZer
				      If my name is too difficult
				      for you, call me Mike :-)
				      e-mail: mailfrom@mail.ru

    Original idea - John Romero (when he was in ID, he created TED5, father of
                    ATED), now he is working in ION Storm

    DJGPP Compiler - DJ Delorie
                     WWW: www.delorie.com

    Allegro graphics library - Shawn Hargreaves and contributors
	                       WWW: talula.demon.co.uk

--------------------------------------------------------------------------------
  Feedback e-mail adress:
    mailfrom@mail.ru
    Subject: [ATED] (Place Your subj here)