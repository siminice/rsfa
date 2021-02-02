import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;

public class FrSeason {

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.magyarfutball.hu/en/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst(".ranking");
    extractClubs(content);
  }

  private static void extractClubs(Element content) {
    Elements rows = content.select(".club");
    for (Element row : rows) {
      Node url = row.selectFirst("a[href]");
      if (url!=null) System.out.println(extractLink(url));
    }
  }

  private static String extractLink(Node n) {
    if (n==null) return "";
    String link = n.attr("href");
    String[] parts = link.split("/");
    int np = parts.length;
    return parts[np-2];
  }
}

