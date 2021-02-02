import java.io.File;
import java.io.IOException;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

public class Season {

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "http://us.soccerway.com/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst("[class=leaguetable sortable table detailed-table]");
    extractReport(content);
  }

  private static void extractReport(Element content) {
    Element tab = content.selectFirst("tbody");
    Elements rows = tab.select("tr");
    for (Element row : rows) {
      Element team = row.selectFirst("[class=text team large-link]");
      Node url = team.selectFirst("a[href]");
      System.out.println(extractLink(url));
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
