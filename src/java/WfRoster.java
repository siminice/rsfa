package org.rsfa.wf;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class WfRoster {

  private static Map<String,String> trans = new HashMap();

  public static void main(String args[]) {
    trans.put("Torwart", "P");
    trans.put("Abwehr", "F");
    trans.put("Mittelfeld", "M");
    trans.put("Sturm", "A");
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.footballatabase.eu");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractRoster(doc);
  }

  private static void extractRoster(Document doc) {
    Element table = doc.selectFirst(".responsive-table");
    Element etbod = table.selectFirst("tbody");
    Elements rows = etbod.children();
    for (Element row : rows) {
      if (row.className().equals("odd") || row.className().equals("even")) {
        Elements cols = row.select("td");
        Element epos = cols.get(0);
        Element epl = cols.get(1);
        String pos = epos.attr("title");
        Element tab = epl.selectFirst("tbody");
        Element tr = tab.selectFirst("tr");
        Elements tc = tr.select("td");
        Element tlink = tc.get(1);
        Element tname = tlink.selectFirst(".hide-for-small");
        Element plink = tname.selectFirst("a[href]");
        String pid = plink.attr("id");
        String pname = plink.childNode(0).toString();
        String link = plink.attr("href");
        System.out.println(String.format("%s,%s,%s,%s", pid, pname,
            trans.getOrDefault(pos, " "), link));
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
