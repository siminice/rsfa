import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

public class Roster {

  private static Map<String,String> trans = new HashMap();

  public static void main(String args[]) {
    trans.put("G", "P");
    trans.put("D", "F");
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "http://us.soccerway.com/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst(".squad-container");
    extractRoster(content);
  }

  private static void extractRoster(Element content) {
    Elements tabs = content.select("table");
    Element squad = tabs.get(0).selectFirst("tbody");
    Elements rows = squad.select("tr");
    for (Element row : rows) {
      Element shirt = row.selectFirst(".shirtnumber");
      String number = shirt != null ? shirt.childNode(0).toString() : " ";
      Element name = row.selectFirst("[class=name large-link]");
      Node url = name.selectFirst("a[href]");
      String link = extractLink(url);
      Element pos = row.selectFirst("[class=position large-link]");
      String post = pos != null ? pos.childNode(0).childNode(0).toString() : " ";
      System.out.println(String.format("%s,%s,%s", trans.getOrDefault(post, post), number, link));
    }
    if (tabs.size() > 1) {
      Element coach = tabs.get(1).selectFirst("tbody");
      Elements crows = coach.select("tr");
      for (Element row : crows) {
        Element name = row.selectFirst("[class=name large-link]");
        Node url = name.selectFirst("a[href]");
        String link = extractLink(url);
        System.out.println(String.format("C, ,%s", link));
      }
    }
  }

  private static String extractLink(Node n) {
    if (n==null) return "";
    String link = n.attr("href");
    String[] parts = link.split("/");
    int np = parts.length;
    return parts[np-2] + "/" + parts[np-1];
  }

}
