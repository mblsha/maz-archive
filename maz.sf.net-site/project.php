<?php
	switch ($lang) {
		case "RU":
			$features    = "Фичи";
			$changes     = "Изменения";
			$version     = "Версия";
			$screenshots = "Скриншоты";
			$files       = "Файлы";
			$currentVersion = "Текущая версия";
			break;
		default:
			$features    = "Features";
			$changes     = "Changes";
			$version     = "Version";
			$screenshots = "Screenshots";
			$files       = "Files";
			$currentVersion = "Current version";
	}

	function startElement($parser, $name, $attrs) {
		global $bgcolor, $bgcolor1, $bgcolor2, $bgcolor3;
		global $projectName;

		global $lang;
		global $file;
		global $features, $changes, $version, $screenshots, $files, $currentVersion;

		switch ($name) {
			case "PROJECT":
				$name = "NAME" . $lang;
				$pageTitle = $attrs[$name];
				require_once ("part1.html");
				$bgcolor = $bgcolor1;
				break;
			case "DESCRIPTION":
				$text = "TEXT" . $lang;
				print "<p>$attrs[$text]</p>\n";
				break;
			case "FEATURES":
				print "<p>\n\t<b>$features:</b><br />\n\t<ul>\n";
				break;
			case "FEATURE":
				$text = "TEXT" . $lang;
				print "\t\t<li>$attrs[$text]</li>\n";
				break;
			case "CHANGES":
				print "<p>\n\t<b>$changes:</b><br />\n";
				break;
			case "VERSION":
				print "\t$version <i>$attrs[NUM]</i>\n\t<ul>\n";
				break;
			case "CHANGE":
				$text = "TEXT" . $lang;
				print "\t\t<li>$attrs[$text]</li>\n";
				break;
			case "SCREENSHOTS":
				print "<p>\n\t<b>$screenshots:</b>\n";
				print "\t<table width=\"100%\" border=\"0\" cellspacing=\"0\"> <tr bgcolor=\"$bgcolor3\"><td>\n";
				break;
			case "PICTURE":
				$desc = "DESC" . $lang;
				print "\t\t<a href=\"image.php?file=$attrs[FILENAME]&desc=$attrs[$desc]&lang=$lang\">";
				print "<img src=\"$attrs[THUMB]\" alt=\"$attrs[$desc]\" /></a>\n";
				break;
			case "FILES":
				print "<p>\n\t<b>$files:</b> ";
				if ($attrs[VERSION] != "")
					print "<i>($currentVersion: $attrs[VERSION])</i>";
				print "\n\t<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n";
				break;
			case "FILE":
				$desc = "DESC" . $lang;
				print "\t\t<tr bgcolor=\"$bgcolor\">\n";
				print "\t\t\t<td><a href=\"$attrs[URL]\">$attrs[NAME]</a></td>\n";
				print "\t\t\t<td>$attrs[$desc]</td>\n";
				print "\t\t</tr>\n";

				if ($bgcolor == $bgcolor1)
					$bgcolor = $bgcolor2;
				else
					$bgcolor = $bgcolor1;
				break;
			case "HEADER":
				$text = "TEXT" . $lang;
				print "\t\t<tr bgcolor=\"$bgcolor3\"><td></td><td><i>$attrs[$text]</i></td></tr>\n";
				break;
		}
	}

	function endElement($parser, $name) {
		global $lang;

		switch ($name)
		{
			case "PROJECT":
				require_once ("part2.html");
				break;
			case "DESCRIPTION":
				print "\n";
				break;
			case "FEATURES":
				print "\t</ul>\n</p>\n\n";
				break;
			case "CHANGES":
				print "</p>\n\n";
				break;
			case "VERSION":
				print "\t</ul>\n";
				break;
			case "SCREENSHOTS":
				print "\t</td></tr></table>\n</p>\n\n";
				break;
			case "FILES":
				print "\t</table>\n</p>\n";
				break;
		}
	}

	$xml_parser = xml_parser_create();
	xml_set_element_handler($xml_parser, "startElement", "endElement");
	if (!($fp = fopen("projects/" . $file . ".xml", "r"))) {
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
?>
