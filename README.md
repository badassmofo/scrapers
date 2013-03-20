My web scrapers

thedoujin-dl:
<pre>
perl thedoujin-dl.pl [IDs]
perl thedoujin-dl.pl 4321 1234 123
</pre>

fakku-dl.pl
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
</pre>

