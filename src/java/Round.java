import java.io.File;
import java.io.IOException;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

public class Round {

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "http://us.soccerway.com/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element maintab = doc.selectFirst(".table-container");
    Element tab = maintab.selectFirst("tbody");
    extractRound(tab);
  }

  private static void extractRound(Element tab) {
    Elements rows = tab.select("tr");
    for (Element row : rows) {
      Element td = row.selectFirst("[class=score-time score]");
      Node link = td.selectFirst("a[href]");
      System.out.println(link.attr("href"));
    }
  }

}
