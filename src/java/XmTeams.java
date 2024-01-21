import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;


public class XmTeams {

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://transfermarkt.us");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractTeams(doc);
  }

  private static void extractTeams(Document doc) {
    Element table = doc.selectFirst(".responsive-table");
    Element etbod = table.selectFirst("tbody");
    Elements rows = etbod.select("tr");
    for (Element row : rows) {
      Elements cols = row.select("td");
      Element ecn = cols.get(1);
      Element ecr = cols.get(2);
      Element etlink = ecn.selectFirst("a[href]");
      Element erlink = ecr.selectFirst("a[href]");
      String tid = etlink.attr("id");
      String tname = etlink.childNode(0).toString();
      String rlink = erlink.attr("href");
      if (tid==null || tid.length()==0) {
        String tlparts[] = rlink.split("/");
        tid = tlparts[4];
      }
      System.out.println(tid+","+tname+","+rlink);
    }
  }

  private static Element findRow(Elements rows) {
    for (Element r : rows) {
      Element th = r.selectFirst("th");
      Element td = r.selectFirst("td");
      if (th!=null && th.childNodeSize() > 0) {
        String title = th.childNode(0).toString();
        if (title.contains("Full name")) return td;
        if (title.contains("Name in home country")) return td;
      }
    }
    return null;
  }

  private static Element selectProp(Elements props, String t) {
    for (Element e : props) {
      if (e.attr("itemprop").equals(t)) return e;
    }
    return null;
  }

}
