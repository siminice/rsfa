import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class FrRound {
  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.magyarfutball.hu/en/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst("#gamelist");
    extractRound(content);
  }

  private static void extractRound(Element content) {
    Elements rows = content.select(".line");
    for (Element row : rows) {
      Element esc = row.selectFirst(".score");
      if (esc != null) {
        Node nsc = esc.selectFirst("a[href]");
        String link = nsc.attr("href");
        System.out.println(link);
      }
    }
  }
}

