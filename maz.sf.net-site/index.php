<?php
	switch ($lang) {
		case "RU":
			$pageTitle      = "Новости";
			$displayAllNews = "Показать все новости";
			break;
		default:
			$pageTitle      = "News";
			$displayAllNews = "Display all news";
	}
	require_once ("part1.html");

	$file = "xml/news.xml";
	$count = 0;
	$displayCount = 5;
	$displayAll = false;

	function startElement($parser, $name, $attrs) {
		global $display;
		global $count;
		global $displayAll;
		global $displayCount;
		global $displayAllNews;
		global $lang;

		if (($count < $displayCount) || ($display == "all")) {
			switch ($name)
			{
				case "NEWS":
					$desc = "DESC" . $lang;
					if ($attrs[$desc] != "")
						print "<p>$attrs[$desc]</p>\n\n";
					break;
				case "ITEM":
					$title = "TITLE" . $lang;
					$date  = "DATE" . $lang;
					print "<p>\n\t<i>$attrs[$date]</i>: <b>$attrs[$title]</b>\n\t<ul>\n";
					break;
				case "EVENT":
					$text = "TEXT" . $lang;
					print "\t\t<li>$attrs[$text]</li>\n";
					break;
			}
		}
		else if ($displayAll == false)
		{
			$displayAll = true;
			print "<a href=\"index.php?display=all&lang=$lang\">$displayAllNews</a>\n";
		}
	}

	function endElement($parser, $name) {
		global $display;
		global $count;
		global $displayAll;
		global $displayCount;
		global $displayAllNews;
		global $lang;

		if (($count < $displayCount) || ($display == "all")) {
			switch ($name) {
				case "ITEM":
					print "\t</ul>\n</p>\n\n";
					$count++;
					break;
			}
		}
	}

	$xml_parser = xml_parser_create();
	xml_set_element_handler($xml_parser, "startElement", "endElement");
	if (!($fp = fopen($file, "r"))) {
		die("could not open XML input");
	}

	while ($data = fread($fp, 4096)) {
		if (!xml_parse($xml_parser, $data, feof($fp))) {
			die(sprintf("XML error: %s at line %d",
			            xml_error_string(xml_get_error_code($xml_parser)),
			            xml_get_current_line_number($xml_parser)));
		}
	}
	xml_parser_free($xml_parser);

	require_once ("part2.html");
?>
