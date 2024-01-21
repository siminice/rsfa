import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class XmRoster {
  private static Map<String, String> trans = new HashMap();

  public XmRoster() {
  }

  public static void main(String[] var0) {
    trans.put("torwart", "P");
    trans.put("abwehr", "F");
    trans.put("mittelfeld", "M");
    trans.put("sturm", "A");
    trans.put("goalkeeper", "P");
    trans.put("defender", "F");
    trans.put("defence", "F");
    trans.put("right-back", "F");
    trans.put("centre-back", "F");
    trans.put("left-back", "F");
    trans.put("midfield", "M");
    trans.put("attacking midfield", "M");
    trans.put("attack", "A");
    trans.put("centre-forward", "A");
    File var1 = new File(var0[0]);
    Document var2 = null;

    try {
      var2 = Jsoup.parse(var1, "UTF-8", "https://www.footballatabase.eu");
    } catch (IOException var4) {
      var4.printStackTrace();
    }

    extractRoster(var2);
  }

  private static void extractRoster(Document var0) {
    Element var1 = var0.selectFirst(".responsive-table");
    Element var2 = var1.selectFirst("tbody");
    for (Element r : var2.children()) {
      Element p = r.selectFirst(".posrela");
      Element t = p.selectFirst("tbody");
      Elements tr = t.select("tr");
      if (tr.size() > 1) {
        Element etl = tr.get(0);
        Element eps = tr.get(1);
        Element el = etl.selectFirst(".hauptlink");
        Element ep = eps.selectFirst("td");
        Element elink = el.selectFirst("a[href]");
        String link = elink.attr("href");
        String name = elink.childNode(0).toString().trim();
        String[] parts = link.split("/");
        String pid = parts.length > 4 ? parts[4] : "";
        String mp = ep.childNode(0).toString().trim();
        String pos = trans.getOrDefault(mp.toLowerCase(), " ");
        System.out.println(String.format("%s,%s,%s,%s", pid, name, pos, link));
      }
    }
  }

  private static String extractLink(Node var0) {
    if (var0 == null) {
      return "";
    } else {
      String var1 = var0.attr("href");
      String[] var2 = var1.split("/");
      int var3 = var2.length;
      return var2[var3 - 1];
    }
  }

}
