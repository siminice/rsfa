import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

public class HuRoster {

  private static Map<String,String> trans = new HashMap();

  public static void main(String args[]) {
    trans.put("goalkeeper", "P");
    trans.put("defender", "F");
    trans.put("midfielder", "M");
    trans.put("forward", "A");
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.magyarfutball.hu/en/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst("#content");
    Element tab = content.selectFirst("table");
    extractRoster(content);
  }

  private static void extractRoster(Element tab) {
    Element squad = tab.selectFirst("tbody");
    Elements rows = squad.select("tr");
    for (Element row : rows) {
      Elements cols = row.select("td");
      if (cols.size() < 7) continue;
      Element name = cols.get(2);
      Element pos = cols.get(3);
      Node url = name.selectFirst("a[href]");
      String link = extractLink(url);
      String post = pos != null && pos.childNodeSize() > 0 ? pos.childNode(0).toString() : " ";
      System.out.println(String.format("%s,%s,%s", getPost(post), " ", link));
    }
  }

  private static String extractLink(Node n) {
    if (n==null) return "";
    String link = n.attr("href");
    String[] parts = link.split("/");
    int np = parts.length;
    return parts[np-2] + "/" + parts[np-1];
  }

  private static String getPost(String p) {
    Set<String> keys = trans.keySet();
    for (String k : keys) {
      if (p.contains(k)) return trans.get(k);
    }
    return " ";
  }

}

