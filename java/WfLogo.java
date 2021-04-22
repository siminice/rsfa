import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;

public class WfLogo {


  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.worldfootball.net");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractLogos(doc);
  }

  private static void extractLogos(Document doc) {
    Element emain = doc.selectFirst("div#relevant-content");
    Element cont = emain.selectFirst(".content");
    Element tab = cont.selectFirst(".standard_tabelle");
    Element bod = tab.selectFirst("tbody");
    Elements rows = bod.select("tr");
    for (Element r : rows) {
      Elements cols = r.select("td");
      if (cols.size()>0) {
        Element col = cols.get(0);
        Element img = col.selectFirst("img");
        String link = img.attr("src");
        String name = img.attr("title");
        String lparts[] = link.split("/");
        String gif = lparts.length > 6 ? lparts[6] : "";
        String gparts[] = gif.split("\\.");
        String id = gparts[0];
        System.out.println(String.format("%s - %s", id, name));
      }
    }
  }
}

