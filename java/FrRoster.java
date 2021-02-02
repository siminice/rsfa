import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class FrRoster {

  private static Map<String,String> trans = new HashMap();

  public static void main(String args[]) {
    trans.put("Goalkeepers", "P");
    trans.put("Defenders", "F");
    trans.put("Midfielders", "M");
    trans.put("Forwards", "A");
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.footballatabase.eu");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst("#roster_details_1");
    extractRoster(content);
  }

  private static void extractRoster(Element tab) {
    Element squad = tab.selectFirst(".start");
    Elements rows = squad.select("div");
    String pos = " ";
    for (Element row : rows) {
      if (row.hasClass("posheader")) {
        String posh = row.childNode(0).toString().trim();
        pos = trans.getOrDefault(posh, " ");
      } else if (row.className().contains("line")) {
        Node url = row.selectFirst("a[href]");
        String link = extractLink(url);
        System.out.println(String.format("%s,%s,%s", pos, " ", link));
      }
    }
  }

  private static String extractLink(Node n) {
    if (n==null) return "";
    String link = n.attr("href");
    String[] parts = link.split("/");
    int np = parts.length;
    return parts[np-1];
  }

}

