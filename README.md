# My web scrapers

## thedoujin-dl:
<pre>
perl thedoujin-dl.pl [IDs]
perl thedoujin-dl.pl 4321 1234 123
</pre>

## fakku-dl.pl
<pre>
perl fakku-dl.pl [options] [name]
Options:
manga/doujinshi - Which section to download from [default: doujinshi]
--f=%format     - Specify a format to save the zip file as [default: [%a] %t (%s) [%l] ]
Format spcifiers:
%a = artist name
%t = title
%s = series
%l = language
%u = translator
perl fakku-dl.pl "--f=[%a] %t (%s) (%l)" manga koe-no-katachi-english doujinshi manga-amputee-vol1-japanese
perl fakku-dl.pl "--f=[%a] %t (%s) (%l)" manga/koe-no-katachi-english doujinshi manga-amputee-vol1-japanese
</pre>

## License

<pre>
        DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
                    Version 2, December 2004 

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net> 

 Everyone is permitted to copy and distribute verbatim or modified 
 copies of this license document, and changing it is allowed as long 
 as the name is changed. 

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 

  0. You just DO WHAT THE FUCK YOU WANT TO.
</pre>

