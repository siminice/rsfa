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

public class WfApps {

  private static Map<String,String> trans = new HashMap();

  public static void main(String args[]) {
    trans.put("Torwart", "P");
    trans.put("Abwehr", "F");
    trans.put("Mittelfeld", "M");
    trans.put("Sturm", "A");
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.worldfootball.net");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractRoster(doc);
  }

  private static void extractRoster(Document doc) {
    Element sidebar = doc.selectFirst(".sidebar");
    Element head = sidebar.selectFirst(".head");
    Elements data = sidebar.select(".data");
    extractBio(head, data);
  }

  private static void extractBio(Element head, Elements data) {

  }

  private static String extractLink(Node n) {
    if (n==null) return "";
    String link = n.attr("href");
    String[] parts = link.split("/");
    int np = parts.length;
    return parts[np-1];
  }

}
